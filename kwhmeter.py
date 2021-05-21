from datetime import datetime
import serial
import mysql.connector
import sys

right_sensor=0
left_sensor=0
spin=0

Array_Meter_Kwh = []
#5 th floor raspberrypi meter code
Meter_starting_number = 501

now = datetime.now()

old_hour = now.hour
old_minute = now.minute

try:    
    ser = serial.Serial("/dev/ttyUSB0", 9600)    
    #ser = serial.Serial("COM12", 9600)
    print("Serial Port Connected")
except Exception as e:
    print("Serial port is not available, Kindly check the connected serial device") 
    print("Oops!",e.__class__,"occurred",now) 
    sys.exit()

for i in range(30):
    Array_Meter_Kwh.insert(i,0)

def add_record():
    try:
        mydb = mysql.connector.connect(
            host = "103.68.33.37",
            user = "veplserver",
            passwd = "Vepl@123",
            database = "oterra",
            auth_plugin='mysql_native_password')

        mycursor = mydb.cursor()
        for i in range(30):
            sql = "INSERT INTO energy (MeterNo,Energy,Time) VALUES (%s,%s,%s)"
            val= (i+Meter_starting_number,Array_Meter_Kwh[i],now)
            mycursor.execute(sql,val)
            mydb.commit()
        print("record Added ",now)
    except Exception as e:
        print("Oops!",e.__class__,"occurred",now)

def update_record():
    try:
        mydb = mysql.connector.connect(
            host = "103.68.33.37",
            user = "veplserver",
            passwd = "Vepl@123",
            database = "oterra",
            auth_plugin='mysql_native_password')

        mycursor = mydb.cursor()
        for i in range(30):
            sql = "UPDATE rooms SET Energy=%s,Time=%s WHERE MeterNo = %s"
            val= (Array_Meter_Kwh[i],now,i+Meter_starting_number)
            print(Array_Meter_Kwh[i],now,i+Meter_starting_number)            
            mycursor.execute(sql,val)
            mydb.commit()
        print("record updated ",now)
    except Exception as e:
        print("Oops!",e.__class__,"occurred",now)

def add_kwh(id,kwh):
    Array_Meter_Kwh[int(id)] = kwh
    
while True:
  
    now = datetime.now()
    new_hour = now.hour
    new_minute = now.minute

    ser.flushInput()
    ser.flushOutput()

    cc=str(ser.readline())

    print("Serial Received : ",cc,now)

    total =(cc[2:][:-5])
    seperated = total.split(',')
    
    Meter_ID = (seperated[0])
    Meter_Kwh = (seperated[1])
    
    add_kwh(Meter_ID,Meter_Kwh)
    
    print("Meter ID = ",int(Meter_ID)+Meter_starting_number,"Kwh = ",Meter_Kwh) 

    if(old_hour != new_hour):  
        old_hour = new_hour
        add_record()
    if(old_minute != new_minute):
        old_minute = new_minute
        update_record()
    
    
  