import errno
import io
import random
import picamera
from PIL import Image, ImageChops
import datetime
import math
import os
import socket

KSSERVER = 'KSHomePvtCloudServer.lan'
parent_dir_path = "/home/pi/FTP/capture/"
prior_image = None
before_recording_sec = 10
after_recording_sec = 10
entropy_calculated_min = 5  # above then this value motion detected
cctvPort = 1234
LOGFILE = "log/KSHomelog.txt"


# cctvServer = '192.168.1.113'


def sendFileToServer():
    retVal = False
    s = socket.socket()
    try:
        cctvServer = socket.gethostbyname(KSSERVER)
        s.connect((cctvServer, cctvPort))
        cnx = s.makefile('wb')
        lfp = open('before.h264', 'rb')
        cnx.write(lfp.read())
        lfp.close()
        lfp = open('after.h264', 'rb')
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


def image_entropy(img):
    """calculate the entropy of an image"""
    # this could be made more efficient using numpy
    histogram = img.histogram()
    histogram_length = sum(histogram)
    samples_probability = [float(h) / histogram_length for h in histogram]
    return -sum([p * math.log(p, 2) for p in samples_probability if p != 0])


def getFileName():
    if not os.path.isdir(parent_dir_path):
        os.mkdir(parent_dir_path)
    subFolderPath = parent_dir_path + str(datetime.datetime.now().strftime("%m_%d_%y"))
    # print("sub folder")
    # print(subFolderPath)
    if not os.path.isdir(subFolderPath):
        os.mkdir(subFolderPath)
    filepath = subFolderPath + "/" + str(datetime.datetime.now().strftime("%H_%M_%S")) + ".h264"
    # print("file path")
    # print(filepath)
    with io.open(filepath, "w") as fp:
        fp.write("hello world")
        fp.close()
    return filepath


def detect_motion(camera):
    global prior_image
    stream = io.BytesIO()
    camera.capture(stream, format='jpeg', use_video_port=True)
    stream.seek(0)
    if prior_image is None:
        prior_image = Image.open(stream)
        return False
    else:
        current_image = Image.open(stream)

        img = ImageChops.difference(prior_image, current_image)
        entropy = image_entropy(img)  # 1.58496250072
        # print('entropy = ' + str(entropy))
        if entropy > entropy_calculated_min:
            # motion detected
            result = True
        else:
            result = False
        # Compare current_image to prior_image to detect motion. This is
        # left as an exercise for the reader!
        # result = random.randint(0, 10) == 0
        # Once motion detection is done, make the prior image the current
        prior_image = current_image
        return result


# list all file if any in backup
# push one by one until all transfer
# no retry on failure. wait for next tern
# delete the successful sending
def sendBackedFile():
    pass

# create unique file and backup the capture
# file name prefix temp_

def backedFile():
    pass


def logFile(msg):
    txt = str(datetime.datetime.now()) + '       :       ' + msg + "\n"
    if not os.path.isdir("log"):
        os.mkdir("log")
    if not os.path.isfile(LOGFILE):
        with io.open(LOGFILE, 'w') as fp:
            fp.write(txt)
            fp.close()
    else:
        with io.open(LOGFILE, 'a') as fp:
            fp.write(txt)
            fp.close()


with picamera.PiCamera() as camera:
    camera.resolution = (1280, 720)
    camera.annotate_text = datetime.datetime.now().strftime('%m-%d-%Y  %H:%M:%S')
    stream = picamera.PiCameraCircularIO(camera, seconds=20)
    camera.start_recording(stream, format='h264')
    try:
        while True:
            camera.wait_recording(1)
            if detect_motion(camera):
                print('Motion detected!')
                logFile('Motion detected!')
                # As soon as we detect motion, split the recording to
                # record the frames "after" motion
                camera.split_recording('after.h264')
                camera.wait_recording(after_recording_sec)
                # Write the 10 seconds "before" motion to disk as well
                stream.copy_to('before.h264', seconds=before_recording_sec)
                stream.clear()
                # Wait until motion is no longer detected, then split
                # recording back to the in-memory circular buffer
                while detect_motion(camera):
                    camera.wait_recording(1)
                print('Motion stopped!')
                logFile('Motion stopped!')
                camera.split_recording(stream)
                if sendFileToServer():
                    sendBackedFile()
                else:
                    backedFile()
    finally:
        camera.stop_recording()
