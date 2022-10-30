import json


class deviceInfo:
    def __init__(self):
        self.deviceName = "KSHOME_DEV"
        self.deviceType = "default"


class mqttInfo:
    def __init__(self):
        self.mqttBrokerAdd = ""
        self.mqttBrokerPort = 0
        self.subTopic = ""
        self.pubTopic = ""


class cloudInfo:
    def __init__(self):
        self.cloudServerAdd = "KSHomePvtCloudServer.lan"
        self.cloudServerPort = 1234
        self.liveStreamingPort = 10001


class deviceConfiguration(deviceInfo, mqttInfo, cloudInfo):
    def __init__(self):
        f = open("conf.json")
        conf = json.load(f)
        # print(conf)
        f.close()
        self.deviceName = conf["deviceInfo"]["deviceName"]
        self.deviceType = conf["deviceInfo"]["deviceType"]
        self.mqttBrokerAdd = conf["mqttInfo"]["mqttBrokerAdd"]
        self.mqttBrokerPort = int(conf["mqttInfo"]["mqttBrokerPort"])
        self.subTopic = conf["mqttInfo"]["subTopic"]
        self.pubTopic = conf["mqttInfo"]["pubTopic"]
        self.cloudServerPort = int(conf["cloudInfo"]["cloudServerPort"])
        self.cloudServerAdd = conf["cloudInfo"]["cloudServerAdd"]
        self.liveCameraPort = int(conf["cloudInfo"]["liveStreamingPort"])

    def printInfo(self):
        print("device name : " + self.deviceName)
        print("device type : " + self.deviceType)
        print("Mqtt port   : " + str(self.mqttBrokerPort))
        print("MQTT broker : " + self.mqttBrokerAdd)
        print("subTopic :" + self.subTopic)
        print("pubTopic :" + self.pubTopic)
        print("cloud server :" + self.cloudServerAdd)
        print("cloud port : " + str(self.cloudServerPort))
        print("Live server Port : " + str(self.liveCameraPort))

    def getConf(self):
        return self


def printInfoconf(conf):
    print("device name : " + conf.deviceName)
    print("device type : " + conf.deviceType)
    print("Mqtt port   : " + str(conf.mqttBrokerPort))
    print("MQTT broker : " + conf.mqttBrokerAdd)
    print("subTopic :" + conf.subTopic)
    print("pubTopic :" + conf.pubTopic)
    print("cloud server :" + conf.cloudServerAdd)
    print("cloud port : " + str(conf.cloudServerPort))
    print("Live server Port : " + str(conf.liveCameraPort))



