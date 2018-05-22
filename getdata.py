import os
import time
import datetime
import glob
from time import strftime
import serial
import json

ser = serial.Serial('/dev/ttyACM0',9600)

def sensorRead():
    lines = ser.readline()
    return lines

while True:
    lines = sensorRead()
    args = lines.split("\n")
    t = args[0]
    data = t.split(" ")
    #data = [0,1,2,3,4,5,6]
    
    if len(data) >= 6:  
        print(data)
        sound = data[0] #average sound
        lux = data[1] #total average light
        g = data[2] #green light
        b = data[3] #blue light
        temp = data[4]
        humid = data[5]

        datetimeWrite = (time.strftime("%Y-%m-%d ") + time.strftime("%H:%M:%S"))
        datetime = time.strftime("%H")
        datetime = datetime + " "
        #print(datetime)
       
        #ser.write(datetime) #send hh to Arduino
        
        #sensor_data = (temp,humid,lux,r,b,g,c,sound) 
        #sensor_data = temp + " " + humid 
        dict1 = temp
        dict2 = humid
        #fin = json.loads(json.dumps([dict1,dict2]))
        #fin = json.dumps([dict1,dict2])
        fin = json.dumps([sound,lux,g,b,temp,humid])
        print(type(fin))
        print((fin))
        time.sleep(1)
 
        try:
            print("Writing")
            os.write(3, fin + "\n")     
        except:         
            print("Failed")

#while True:
	##date1 = time.strftime("%Y-%m-%d ") 
	#date1 = time.strftime("%S ") 
	#date2 = time.strftime("%H:%M:%S")
	#datetimeWrite = (time.strftime("%Y-%m-%d ") + time.strftime("%H:%M:%S"))
	
	#try:
		#print('{0}\n{1}\n{2}'.format(date1,date2,datetimeWrite))
	#except RuntimeError as e:     
		#print("error\n{0}".format(e))
	#except:
		#print("erro\nFailed to read sensor data")
	#os.write(3,date1+ "\n") 
	##os.write(3,'{"dt": "This is a test"}' + "\n")
	
