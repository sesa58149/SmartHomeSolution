#from playsound import playsound
import paho.mqtt.client as mqtt
import os
from gpiozero import LED, Button
from time import sleep
led = LED(25)
button = Button(23)

led.off()

def on_disconnect(client, userdata, rc):
    if rc != 0:
        print("Unexpected disconnection.")

def on_connect(client, userdata, flags, rc):
    print(f"Connected with result code {rc}")


def sendMessage():
	
	client = mqtt.Client()
	client.on_connect = on_connect
	client.connect("192.168.1.50", 1883, 60)
	client.will_set('doorbell/ring', b'{"status": "off"}')
	client.on_disconnect = on_disconnect
	client.publish('doorbell/ring', payload="pressed", qos=0, retain=False)	
	sleep(3)
	client.disconnect()

while True:
	led.on()
	sleep(2)
	button.wait_for_press()
	#playsound('/home/pi/FTP/Project/CantinaBand3.wav')
	#client.publish('doorbell/ring', payload="pressed", qos=0, retain=False)		
	sendMessage()
	os.system('aplay /home/pi/FTP/Project/CantinaBand3.wav')
	led.off()
	sleep(2)
