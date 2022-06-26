# subscriber.py
import paho.mqtt.client as mqtt
import os
import subprocess
import logging
from time import sleep

def on_connect(client, userdata, flags, rc):
    print(f"Connected with result code {rc}")
    # subscribe, which need to put into on_connect
    # if reconnect after losing the connection with the broker, it will continue to subscribe to the raspberry/topic topic
    client.subscribe("doorbell/ring")
    logger.error(f"Connected with result code {rc}")

# def start --------------------------------------------------------------------
# the callback function, it will be triggered when receiving messages
def on_message(client, userdata, msg):
    print(f"{msg.topic} {msg.payload}")
    logger.info(f"{msg.topic} {msg.payload}")
    cnt=0
    while(cnt != 3):
        os.system('sudo aplay /usr/KHomeApp/CantinaBand.wav')
        #subprocess.call("/usr/KHomeApp/playSound.sh", shell=True)
        os.system('/usr/KHomeApp/textToAudioMainScript.sh')
	#os.system('./textToAudioMainScript.sh')
        cnt=cnt+1
        sleep(2)
        logger.info("Music played")
# def END ----------------------------------------------------------------------

logger = logging.getLogger('KHomeLogger')
logger.setLevel(logging.INFO)
handler = logging.FileHandler("/var/log/doorbellNotofication.log")
#logging.basicConfig(filename="/var/log/doorbellNotofication.log")
logger.addHandler(handler)

client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

# set the will message, when the Raspberry Pi is powered off, or the network is interrupted abnormally, it will send the will message$
client.will_set('raspberry/status', b'{"status": "Off"}')

#print("connecting to the mqtt broker")
logger.info ("connecting to the mqtt broker")

# create connection, the three parameters are broker address, broker port number, and keep-alive time respectively
client.connect("localhost", 1883, 60)

# set the network loop blocking, it will not actively end the program before calling disconnect() or the program crash
client.loop_forever()

