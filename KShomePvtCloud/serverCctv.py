#SOcket programing for Image/vedio transfer
#store the video/Image in to the storage device'
#provide a user interface over https to view the video/images;

import os
import io
import datetime
import time
import socket

dir_pth = '/media/pi/Elements/KSHome/capture/'

cctvPort = 1234

#create tcp socket
cctvSocket = socket.socket()
cctvSocket.bind(('', cctvPort))
cctvSocket.listen(5)
print("socket start listening")

def getSotrageDst(clientAdd):
    fullPath = ' '
    if os.path.isdir('/media/pi'):
        if not os.path.isdir(dir_pth):
            os.mkdir(dir_pth)
        subDir = dir_pth + clientAdd + '/'
        if not os.path.isdir(subDir):
                os.mkdir(subDir)
        subDir1 = subDir + str(datetime.datetime.now().strftime("%m_%d_%y"))
        if not os.path.isdir(subDir1):
                os.mkdir(subDir1)
        fullPath = subDir1 + '/' + str(datetime.datetime.now().strftime("%H_%M_%S")) + '.h264'
    return fullPath
#accept the client connection and make connetion as file like object
while True:
    clientCnx, add = cctvSocket.accept()
    cnx = clientCnx.makefile('rb')
    print("connection arrived")
    #read data from socket and store in disk
    filePath=getSotrageDst( (socket.gethostbyaddr(add[0])[0]).split(".", 1)[0])
    if len(filePath):
        with io.open(filePath,'wb') as fp:
            fp.write( cnx.read())
            fp.close
    cnx.close


    

