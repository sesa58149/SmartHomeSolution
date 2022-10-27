#!/usr/bin/python3
import socket
import threading
import time

import numpy as np

from picamera2 import Picamera2
from picamera2.encoders import H264Encoder
from picamera2.outputs import CircularOutput, FileOutput
import paho.mqtt.client as mqtt

mqttBrokerAdd = "192.168.1.50"
mqttBrokerPort = 1883
subTopic = "KSHome/Armed/camera"
isAlrmed = False
KSSERVER = 'KSHomePvtCloudServer.lan'
cctvPort = 1234

MIN_PIXEL_DIFF = 15.0
MAX_PRE_DET_WINDOW_SEC = 10
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


def on_connect(client, userdata, flags, rc):
    print("Client connected to the Broker")
    if rc == 0:
        mqttClient.subscribe("KSHome/Armed/camera", 0)


def on_message(client, userdata, message):
    global isAlrmed
    rxMsg = str(message.payload.decode("utf-8"))
    print("message received : ", rxMsg)
    msgTopic = message.topic
    print("message topic : ", msgTopic)
    print("message qos=", message.qos)
    print("message retain flag=", message.retain)
    if msgTopic == "KSHome/Armed/camera":
        if rxMsg == "ON\r\n" or rxMsg == "on\r\n" or rxMsg == "ON" or rxMsg == "on":
            isAlrmed = True
        elif rxMsg == "OFF\r\n" or rxMsg == "off\r\n" or rxMsg == "OFF" or rxMsg == "off":
            isAlrmed = False

    print("notification status :", isAlrmed)


mqttClient = mqtt.Client('LoftCamera')
mqttClient.on_connect = on_connect
mqttClient.connect(mqttBrokerAdd, mqttBrokerPort, 60)
mqttClient.on_message = on_message
mqttClient.publish("KSHome/Camera", "disArmed")
mqttClient.loop_start()


def sendFileToServer():
    retVal = False
    s = socket.socket()
    try:
        cctvServer = socket.gethostbyname(KSSERVER)
        s.connect((cctvServer, cctvPort))
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


t = threading.Thread(target=server)
t.setDaemon(True)
t.start()
isStarted = False
isEnd = False
motionDetected = False
captureStart = True
waitTime = 0
prev = None
startTime = 0


def motionDetection():
    global isStarted, motionDetected, isEnd, captureStart, prev, waitTime, startTime
    cur = picam2.capture_buffer("lores")
    cur = cur[:w * h].reshape(h, w)
    if captureStart:
        if prev is not None:
            # Measure pixels differences between current and
            # previous frame
            mse = np.square(np.subtract(cur, prev)).mean()
            if mse > MIN_PIXEL_DIFF:
                if not isStarted:
                    isStarted = True
                    print("New Motion", mse)
                    epoch = int(time.time())
                    # circ.fileoutput = "{}.h264".format(epoch)
                    circ.fileoutput = "tmp.h264"
                    circ.start()
                    startTime = time.time()
                    motionDetected = True
                else:
                    if time.time() - startTime > MAX_PRE_DET_WINDOW_SEC:
                        isEnd = True
            else:
                if motionDetected and isStarted and time.time() - startTime > MAX_PRE_DET_WINDOW_SEC:
                    isEnd = True
            if isEnd:
                circ.stop()
                isEnd = False
                isStarted = False
                motionDetected = False
                captureStart = False
                waitTime = time.time()
                print("Stop recording ", time.time() - startTime)
                sendFileToServer()
                print("capture saved on Server")

    else:  # wait for the circular buffer to fill upto window size
        if time.time() - waitTime > MAX_PRE_DET_WINDOW_SEC:
            captureStart = True
    prev = cur

#isAlrmed = True
while True:
    if isAlrmed:
        motionDetection()
    else:
        time.sleep(5)
        print("camera is unalrmed")
picam2.stop_encoder()
