HOME_DIR = "/home/kshome/app/"
LOGFILE = "KSHomeLog.txt"
import os
from datetime import datetime


class systemLogMng:
    def __init__(self):
        if not os.path.exists(HOME_DIR + LOGFILE):
            fp = open(HOME_DIR + LOGFILE, "w")
            fp.close()

    def logFile(self, msg):
        fp = open(HOME_DIR + LOGFILE, "a")
        logMsg = msg + " : " + str(datetime.now())
        fp.write(logMsg + "\n")
        fp.close()

    def logFileReset(self):
        fp = open(HOME_DIR + LOGFILE, "w")
        fp.close()
