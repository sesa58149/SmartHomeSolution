from gpiozero import LED, Button
from time import sleep
from threading import Thread, Condition


class IOTaskClass:
    def __init__(self, notityC):
        self.led = LED(25)
        self.button = Button(23)
        self.ledStatus = 0  # LED initial state is off
        self.led.off()
        self.condition = Condition()
        self.outT = Thread(target=self.ledTask)
        self.inT = Thread(target=self.buttonTask)
        self.notifyCallback = notityC

    def buttonTask(self):
        while True:
            self.button.wait_for_press()
            #self.condition.notify_all()
            self.notifyCallback.sendNotification('pressed')
            sleep(5)

    def ledTask(self):
        while True:
            if self.ledStatus == 0:  # Off Led
                self.led.off()
            elif self.ledStatus == 1:  # On LED
                self.led.on()
            elif self.ledStatus == 2:  # Toggle LED
                self.led.toggle()
            else:
                print("wrong LED command")
            sleep(1)

    def startIOTask(self):
        self.outT.start()
        self.inT.start()
