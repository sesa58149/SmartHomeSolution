from gpiozero import LED, Button
from time import sleep
from threading import Thread, Condition


class IOTaskClass:
    def __init__(self, notityC):

        self.button = Button(23)
        self.ledStatus = 0  # LED initial state is off
        self.led1 = LED(24)
        self.led2 = LED(25)
        self.led3 = LED(8)
        self.led4 = LED(12)
        self.condition = Condition()
        self.outT = Thread(target=self.ledTask)
        self.inT = Thread(target=self.buttonTask)
        self.notifyCallback = notityC

    def setLedState(self, state):
        self.ledStatus = state

    def getLedState(self):
        return self.ledStatus

    def motionDetectLEDPatter(self):
        self.led1.on()
        self.led2.on()
        self.led3.on()
        self.led4.on()
        sleep(.5)
        self.led1.off()
        self.led2.off()
        self.led3.off()
        self.led4.off()

    def buttonPressedLEDPattern(self):
        self.led1.on()
        self.led2.off()
        self.led3.off()
        self.led4.off()
        sleep(.5)
        self.led1.off()
        self.led2.on()
        self.led3.off()
        self.led4.off()
        sleep(.5)
        self.led1.off()
        self.led2.off()
        self.led3.on()
        self.led4.off()
        sleep(.5)
        self.led1.off()
        self.led2.off()
        self.led3.off()
        self.led4.on()
        sleep(.5)
        self.led1.off()
        self.led2.off()
        self.led3.off()
        self.led4.off()

    def buttonTask(self):
        while True:
            self.ledStatus = 0
            self.button.wait_for_press()
            self.ledStatus = 3
            self.notifyCallback.sendNotification('pressed')
            sleep(10)

    def ledTask(self):
        while True:
            if self.ledStatus == 0:  # Off Led
                self.led1.off()
            elif self.ledStatus == 1:  # On LED
                self.led1.on()
            elif self.ledStatus == 2:  # Toggle LED
                self.led1.toggle()
            elif self.ledStatus == 3:  # Toggle LED
                self.buttonPressedLEDPattern()
            elif self.ledStatus == 4:  # Toggle LED
                self.motionDetectLEDPatter()
            else:
                print("wrong LED command")
            sleep(.5)

    def startIOTask(self):
        self.outT.start()
        self.inT.start()
