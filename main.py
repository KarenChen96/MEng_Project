#1. This is the main function
#2. It uses picam module

import picamera
import P3picam
#from sleep import time
from datetime import datetime
from subprocess import call
import time
import os
import json

motionstate = False #record if there is a motion in this time
picPath = "/home/pi/camera/images/"

period = 10 #set a period for motion detecting
limit = 5 #set a maximum value, if the frequency of motoin is beyond this value, alarm!
lastsave = time.time()
count = 0

def captureImage(currentTime, picPath):
    picName = currentTime.strftime("%Y.%m.%d-%H%M%S") + '.jpg'
    with picamera.PiCamera() as camera:
        camera.resolution = (1280, 720)
        camera.capture(picPath + picName)
    print ("We have taken the picture!")
    return picName

def getTime():
    currentTime = datetime.now()
    return currentTime

#strftime: string for time
def timeStamp(currentTime, picPath, picName):
    #varaible for file path
    filepath = picPath + picName
    #create methods to stamp on picture 
    message = currentTime.strftime("%Y.%m.%d - %H%M%S") + '.jpg'
    #create command to execute
    timestampCommand = "/usr/bin/convert " + filepath + " -pointsize 36 \
    -fill white -annotate +750+680 ' " + message + "' " + filepath 
    #execute the command
    call ([timestampCommand], shell = True)
    #print ("We have timestamped our picture.")

while True:
    starttime = time.time()
    motionstate = P3picam.motion() 
    
    if motionstate:
		#bicomunication test
	    test1 = "normal"
	    test2 = "not"
	    fin = json.dumps([test1, test2])
	    time.sleep(1)
	    try:
			print("Writing")
			os.write(3, fin + "\n")
	    except:
			print("Failed")
		
        print ("Motion detect!")
        currentTime = getTime()
        picName = captureImage(currentTime, picPath)
        timeStamp(currentTime, picPath, picName)
        ins = "The patient is normal."

        count +=1
        print (time.time() - lastsave)
                     
        if (time.time() - lastsave <= 30 and count >= 3):
			print("THe patient is uncomfortable!")
			lastsave = time.time()
			count = 0
					   
    if(time.time() - lastsave > 30):
        lastsave = time.time()
        count= 0
                #camera.resolution = (1280,720)
                #camera.capture("motionImage.jpg") #take only one picture? 
            #previous one is replaced by the later one?
            #camear.stop_preview()
        #print "pictures taken."
