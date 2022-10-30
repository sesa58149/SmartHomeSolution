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

# device configuration
# mqttBrokerAdd = "192.168.1.50"
# mqttBrokerPort = 1883
# subTopic = "KSHome/Armed/camera"
# KSSERVER = 'KSHomePvtCloudServer.lan'
# cctvPort = 1234
HOME_DIR = "/usr/KSHome/"
LOGFILE = "KSHomeLog.txt"
MIN_PIXEL_DIFF = 15.0
MAX_PRE_DET_WINDOW_SEC = 10

# Camera configuration
lsize = (320, 240)
picam2 = Picamera2()
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

w, h = lsize
prev = None
encoding = False
ltime = 0

isAlrmed = False
loadDeviceConf = kshomeConfMgr.deviceConfiguration()

devConf = loadDeviceConf.getConf()


# kshomeConfMgr.printInfoconf(devConf)


def logFile(msg):
    fp = 0
    if os.path.exists(HOME_DIR + LOGFILE):
        fp = open(HOME_DIR + LOGFILE, "a")
    else:
        fp = open(HOME_DIR + LOGFILE, "w")
    logMsg = msg + " : " + str(datetime.now())
    fp.write(logMsg + "\n")
    fp.close()


def on_connect(client, userdata, flags, rc):
    print("Client connected to the Broker")
    if rc == 0:
        mqttClient.subscribe(devConf.subTopic, 0)


def on_message(client, userdata, message):
    global isAlrmed
    rxMsg = str(message.payload.decode("utf-8"))
    print("message received : ", rxMsg)
    msgTopic = message.topic
    print("message topic : ", msgTopic)
    print("message qos=", message.qos)
    print("message retain flag=", message.retain)
    if msgTopic == devConf.subTopic:
        if rxMsg == "ON\r\n" or rxMsg == "on\r\n" or rxMsg == "ON" or rxMsg == "on":
            isAlrmed = True
        elif rxMsg == "OFF\r\n" or rxMsg == "off\r\n" or rxMsg == "OFF" or rxMsg == "off":
            isAlrmed = False

    print("notification status :", isAlrmed)


def sendFileToServer():
    retVal = False
    s = socket.socket()
    try:
        cctvServer = socket.gethostbyname(devConf.cloudServerAdd)
        s.connect((cctvServer, devConf.cloudServerPort))
        cnx = s.makefile('wb')
        lfp = open('tmp.h264', 'rb')
        cnx.write(lfp.read())
        lfp.close()
        retVal = True

    except socket.error as exception:
        if exception.errno == errno.ECONNREFUSED:
            retVal = False
        print("failed to send over NW")
        logFile('failed to send over NW')

    finally:
        s.close()

    return retVal


def server():
    global circ, picam2
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
        sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        sock.bind(("0.0.0.0", 10001))
        sock.listen()
        while tup := sock.accept():
            print("connection accepted")
            event = threading.Event()
            conn, addr = tup
            stream = conn.makefile("wb")
            filestream = FileOutput(stream)
            filestream.start()
            picam2.encoder.output = [circ, filestream]
            filestream.connectiondead = lambda ex: event.set()
            event.wait()


mqttClient = mqtt.Client(devConf.deviceName)
mqttClient.on_connect = on_connect
mqttClient.connect(devConf.mqttBrokerAdd, devConf.mqttBrokerPort, 60)
mqttClient.on_message = on_message
mqttClient.publish("KSHome/Camera", "disArmed")
mqttClient.loop_start()

t = threading.Thread(target=server)
t.setDaemon(True)
t.start()

prev = None
startTime = 0
logFile("Service started ")


def motionDetection():
    global prev, startTime
    cur = picam2.capture_buffer("lores")
    cur = cur[:w * h].reshape(h, w)

    if prev is not None:
        # Measure pixels differences between current and
        # previous frame
        mse = np.square(np.subtract(cur, prev)).mean()
        if mse > MIN_PIXEL_DIFF:
            print("New Motion", mse)
            epoch = int(time.time())
            # circ.fileoutput = "{}.h264".format(epoch)
            circ.fileoutput = "tmp.h264"
            circ.start()
            startTime = time.time()
            while time.time() - startTime < MAX_PRE_DET_WINDOW_SEC:
                time.sleep(1)

            circ.stop()
            print("Stop recording ", time.time() - startTime)
            sendFileToServer()
            print("capture saved on Server")
            # wait for the MAX_PRE_DET_WINDOW_SEC to get circular buffer 1/2 vedio
            startTime = time.time()
            while time.time() - startTime < MAX_PRE_DET_WINDOW_SEC - 1:
                time.sleep(1)
            cur = None  # make sure next time get latest capture not the previous detected one

    prev = cur


while True:
    if isAlrmed:
        motionDetection()
    else:
        time.sleep(5)
        print("camera is unalrmed")
picam2.stop_encoder()
