from abc import ABC, abstractmethod
from threading import Thread


class serviceInterface(ABC):
    def __init__(self, name, stype):
        self.serviceName
        self.ThreadId
        self.serviceState
        self.serviceStatus
        self.serviceDependency
        self.AsSerivce

    @abstractmethod
    def threadStateMachine(self):
        pass

    @abstractmethod
    def configure(self, conf=None):
        pass

    @abstractmethod
    def start(self, param=None):
        pass

    @abstractmethod
    def reStart(self, param=None):
        pass

    @abstractmethod
    def addDependency(self, services):
        pass

    def getDependency(self):
        return self.serviceDependency

    @abstractmethod
    def stop(self):
        pass
