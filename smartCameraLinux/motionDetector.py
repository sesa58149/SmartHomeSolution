from gpiozero import  Button
from time  import sleep
import paho.mqtt.client as mqtt
import  datetime

mSensor = Button(16)
if (mSensor.pull_up == True):
	print("Pull UP")
else:
	print("Pull Low")

print(mSensor.pull_up)

def on_connect(client, userdata, flags, rc):
    print(f"Connected with result code {rc}")

def sendNotification() :
	client = mqtt.Client()
	client.on_connect = on_connect
	client.connect("192.168.1.50", 1883, 60)
	dt = datetime.datetime.now()
	msg = str(dt) + ": detected"
	pubObj = client.publish('motion/leavingRoom', payload=msg, qos=1, retain=False)
	#pubObj.wait_for_publish()
	sleep(10)
	client.disconnect()
	print( "Notification sent")

print ("Motion detector started")

detected = False
while True:
	if mSensor.value == 1:
		print("No Motion")
		detected=False
	elif mSensor.value == 0:
		if detected == False:
			print("Motion Detected")
			sendNotification()
			detected=True

	print(mSensor.value)
	sleep(1)
