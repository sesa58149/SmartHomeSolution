from kshomeServicesIf import serviceInterface
from time import sleep
from threading import Thread

states = ["None", "init", "config", "execute"]


class configService(serviceInterface):
    def __init__(self, name, isService):
        self.serviceDependency = []
        self.serviceName = name
        self.AsService = isService
        self.serviceState = None
        self.serviceStatus = None
        self.ThreadClose = False
        if isService:
            self.ThreadId = Thread(target=self.threadStateMachine)

    def configure(self, conf):
        print("configuration")

    def reStart(self, param=None):
        print("restart")
        self.serviceState = states[1]

    def threadStateMachine(self):
        self.serviceState = states[1]
        while not self.ThreadClose:
            if self.serviceState == states[1]:
                print("init state")
                self.serviceState = states[2]
            elif self.serviceState == states[2]:
                print("config state")
                self.serviceState = states[3]
            elif self.serviceState == states[3]:
                print("execute state")

            sleep(5)
        print("Exiting from the thread")

    def start(self, param=None):
        if self.AsService:
            self.ThreadId.start()
        else:
            print("Not a service")

    def addDependency(self, services):
        for s in services:
            self.serviceDependency.append(s)

    def stop(self):
        if self.AsService:
            self.ThreadClose = True


service = configService("configuration", True)
service.addDependency(["scm", "sdm", "sm", "cm", "dm"])
service.configure("a")
print(service.getDependency())
service.start()
print(service.serviceName)

sleep(30)
service.stop()
sleep(10)
print("Exit from Process")
# service.start(param=data)
# service.reStart()
# print(service.getServiceStates)
