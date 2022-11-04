#!/usr/bin/python3
import os.path
import socket
import threading
import time
from datetime import datetime
import numpy as np

from picamera2 import Picamera2
from picamera2.encoders import H264Encoder
from picamera2.outputs import CircularOutput, FileOutput
import paho.mqtt.client as mqtt
import kshomeConfMgr
from kshomeDIOMgr import IOTaskClass
from kshomeSystemLog import systemLogMng, HOME_DIR

# device configuration
# mqttBrokerAdd = "192.168.1.50"
# mqttBrokerPort = 1883
# subTopic = "KSHome/Armed/camera"
# KSSERVER = 'KSHomePvtCloudServer.lan'
# cctvPort = 1234


# Camera configuration
lsize = (320, 240)
picam2 = Picamera2()
circ = None


def cameraInit():
    global circ
    # main={"size": (640, 480), "format": "RGB888"}
    video_config = picam2.create_video_configuration(main={"size": (1280, 720), "format": "RGB888"},
                                                     lores={"size": lsize, "format": "YUV420"})
    picam2.configure(video_config)
    picam2.start_preview()
    encoder = H264Encoder(1000000, repeat=True)
    circ = CircularOutput(buffersize=300)
    encoder.output = [circ]
    picam2.encoder = encoder
    picam2.start()
    picam2.start_encoder()


class notificationService(systemLogMng):
    def __init__(self, mqttConf):
        self.isArmed = False
        self.subList = mqttConf.subTopic
        self.pubList = mqttConf.pubTopic
        self.mqttClient = mqtt.Client(mqttConf.deviceName)
        self.mqttClient.on_connect = self.on_connect
        self.mqttClient.on_message = self.on_message
        self.mqttClient.connect(devConf.mqttBrokerAdd, devConf.mqttBrokerPort, 60)

    def startTxRx(self):
        self.mqttClient.loop_start()

    def eventSub(self, sList):
        for s in sList:
            self.subList.append(s)

    def eventPub(self, pList):
        for p in pList:
            self.pubList.append(p)

    def on_connect(self, client, userdata, flags, rc):
        print("Client connected to the Broker")
        if rc == 0:
            for s in self.subList:
                self.mqttClient.subscribe(s, 0)

    def on_message(self, client, userdata, message):

        rxMsg = str(message.payload.decode("utf-8"))
        print("message received : ", rxMsg)
        msgTopic = message.topic
        print("message topic : ", msgTopic)
        print("message qos=", message.qos)
        print("message retain flag=", message.retain)
        for t in self.subList:
            if msgTopic == t:
                if t == "doorbell/camera/motion":
                    self.subCallbackMotion(rxMsg)
                    print(f"message from subscription:      {t}")
                elif t == "doorbell/camera/armed":
                    self.subCallbackArmed(rxMsg)
                    print(f"message from subscription:      {t}")
                elif t == "doorbell/resetlogfile":
                    self.subCallbackResetLogFile(rxMsg)
                    print(f"message from subscription:      {t}")
                else:
                    print("No subscription found ")
                    print(f"message from subscription:      {t}")


    def subCallbackMotion(self, rxMsg):
        pass

    def subCallbackArmed(self, rxMsg):
        if rxMsg == "ON\r\n" or rxMsg == "on\r\n" or rxMsg == "ON" or rxMsg == "on":
            self.isArmed = True
        elif rxMsg == "OFF\r\n" or rxMsg == "off\r\n" or rxMsg == "OFF" or rxMsg == "off":
            self.isArmed = False

    def subCallbackResetLogFile(self, rxMsg):
        if bool(rxMsg):
            self.logFileReset()

    def armedStatus(self):
        return self.isArmed

    def sendNotification(self, msg):
        self.mqttClient.publish(self.pubList[0], payload=msg, qos=0, retain=False)


class cloudServer(systemLogMng):
    def __init__(self, serverAdd, serverPort):
        self.add = serverAdd
        self.port = serverPort

    def sendFileToCloud(self):
        retVal = False
        s = socket.socket()
        try:
            cctvServer = socket.gethostbyname(self.add)
            s.connect((cctvServer, self.port))
            cnx = s.makefile('wb')
            lfp = open(HOME_DIR + 'tmp.h264', 'rb')
            cnx.write(lfp.read())
            lfp.close()
            retVal = True

        except socket.error as exception:
            if exception.errno == errno.ECONNREFUSED:
                retVal = False
            print("failed to send over NW")
            self.logFile('failed to send over NW')

        finally:
            s.close()

        return retVal


class videoStreaming:
    def __init__(self, port):
        self.streamThread = None
        self.circularBuf = None
        self.camModule = None
        self.tcpPort = port

    def launchStreaming(self):
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
            sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
            sock.bind(("0.0.0.0", self.tcpPort))
            sock.listen()
            while tup := sock.accept():
                print("connection accepted")
                event = threading.Event()
                conn, addr = tup
                stream = conn.makefile("wb")
                filestream = FileOutput(stream)
                filestream.start()
                self.camModule.encoder.output = [self.circularBuf, filestream]
                filestream.connectiondead = lambda ex: event.set()
            event.wait()

    def startVideoStreaming(self, cBuf, pCam):
        self.circularBuf = cBuf
        self.camModule = pCam
        self.streamThread = threading.Thread(target=self.launchStreaming)
        self.streamThread.setDaemon(True)
        self.streamThread.start()


class motionDetection(cloudServer):
    def __init__(self, lowSize, cloudConf):
        cloudServer.__init__(self, cloudConf.cloudServerAdd, cloudConf.cloudServerPort)
        self.prev = None
        self.startTime = 0
        self.cur = None
        self.w, self.h = lowSize

    def detect(self, camModule, circBuf):
        self.cur = camModule.capture_buffer("lores")
        self.cur = self.cur[:self.w * self.h].reshape(self.h, self.w)

        if self.prev is not None:
            # Measure pixels differences between current and
            # previous frame
            mse = np.square(np.subtract(self.cur, self.prev)).mean()
            if mse > MIN_PIXEL_DIFF:
                print("New Motion", mse)
                epoch = int(time.time())
                # circ.fileoutput = "{}.h264".format(epoch)
                circBuf.fileoutput = HOME_DIR + "tmp.h264"
                circBuf.start()
                self.startTime = time.time()
                while time.time() - self.startTime < MAX_PRE_DET_WINDOW_SEC:
                    time.sleep(1)

                circBuf.stop()
                print("Stop recording ", time.time() - self.startTime)
                self.sendFileToCloud()
                print("capture saved on Server")
                # wait for the MAX_PRE_DET_WINDOW_SEC to get circular buffer 1/2 vedio
                self.startTime = time.time()
                while time.time() - self.startTime < MAX_PRE_DET_WINDOW_SEC - 1:
                    time.sleep(1)
                self.cur = None  # make sure next time get latest capture not the previous detected one

        self.prev = self.cur


loadDeviceConf = kshomeConfMgr.deviceConfiguration()
devConf = loadDeviceConf.getConf()

MIN_PIXEL_DIFF = devConf.minPixelDiff
MAX_PRE_DET_WINDOW_SEC = devConf.maxPreDetWinSec

sysLog = systemLogMng()
sysLog.logFile("Service started ")

cameraInit()

eventClass = notificationService(devConf)
eventClass.startTxRx()

if devConf.deviceType == "Advance Camera Module":
    ioHandler = IOTaskClass(eventClass)
    ioHandler.startIOTask()

liveVideo = videoStreaming(10001)
liveVideo.startVideoStreaming(circ, picam2)

motionDet = motionDetection(lsize, devConf)
while True:
    if eventClass.armedStatus():
        motionDet.detect(picam2, circ)
    else:
        time.sleep(5)
        print("camera is unalrmed")
picam2.stop_encoder()
