import board
import time
import datetime
import busio
import csv
import socket
from digitalio import DigitalInOut
from adafruit_pn532.i2c import PN532_I2C

#tcp
s=socket.socket() #AF_INET, SOCK_STREAM
address = ("", 2500)
s.bind(address)
s.listen(1)
print('Waiting...')
c_socket, c_addr = s.accept()
print("Connection from ", c_addr)

#uid
uid1 = ['0xf1', '0x50', '0x3b', '0x21'] #Kim
uid2 = ['0x46', '0x73', '0xcb', '0xb2'] #Lee
uid3 = ['0x96', '0x34', '0xdc', '0xb2'] #Park
uid4 = ['0x8d', '0x31', '0xfe', '0x44'] #Choi
uid5 = ['0xb6', '0x3', '0xc8', '0xb2'] #Jo
uid6 = ['0xb6', '0xc1', '0xe6', '0xb2'] #Han


#csvtable
#id = 1 #id value entering column > 0
Tdate = [0, 10, 20, 30, 40] # Values in Column 1 to distinguish dates
t = [1, 2, 3, 4, 5, 6, 7, 8, 9] #worktime = [9, 10, 11, 12, 1, 2, 3, 4, 5]
date = 0 #find Tdate value

#setting nfc
i2c = busio.I2C(board.SCL, board.SDA)
reset_pin = DigitalInOut(board.D6)
req_pin = DigitalInOut(board.D12)
pn532 = PN532_I2C(i2c, debug=False, reset=reset_pin, req=req_pin)
ic, ver, rev, support = pn532.firmware_version
print("Found PN532 with firmware version: {0}.{1}".format(ver, rev))
pn532.SAM_configuration()

#Make csv file array
ins = open("1st week of December_schedule.csv", "r") #1st week of December_schedule
#ins = open("December 2nd week_schedule.csv", "r") #December 2nd week_schedule
#ins = open("December 3rd week_schedule.csv", "r") #December 3rd week_schedule
#ins = open("December 4th week_schedule.csv", "r") #December 4th week_schedule
st = [] #schedules Table

for line in ins:
    row = line.rstrip().split(',')
    st.append(row)

#Check Today's Date
now = datetime.datetime.now()
nowDate = now.strftime('%Y-%m-%d')
nowTime = now.strftime('%H')

#Find Today's Date
for i in Tdate :
   if st[i][0] == nowDate :
      date = i

print("Waiting for RFID/NFC card...")
#Find nfc
while True :

    uid = pn532.read_passive_target(timeout=0.5)
     
    if uid is None:
        continue
        
    uidn = [hex(i) for i in uid]
    print(uidn)
    n=0
    
#Find uid
    if uidn == uid1 : #uid1 differentiation
        print("uid1:", uidn) #uidn is change a name or ipadress
        n = 1
        
    if uidn == uid2 : #uid2 differentiation
        print("uid2:", uidn)
        n = 2
        
    if uidn == uid3 : #uid3 differentiation
        print("uid3:", uidn)
        n = 3      

    if uidn == uid4 : #uid4 differentiation
        print("uid4:", uidn)
        n = 4
        
    if uidn == uid5 : #uid5 differentiation
        print("uid5:", uidn)
        n = 5
        
    if uidn == uid6 : #uid6 differentiation
        print("uid6:", uidn)
        n = 6
        
    if n == 0 :
        ncard = "This card is not registered.\n"
        print("This card is not registered.\n")
        c_socket.send(ncard.encode())
        time.sleep(1)
        print("Waiting for RFID/NFC card...")
        continue
        
    uidprint = st[0][n] + " came to work.\n"
    c_socket.send(uidprint.encode())
    print(uidprint)

#print schedule
    for i in t :
      schedule = st[date + i][0]+ ":"+ st[date + i][n] +"\n"
      print(schedule)
      c_socket.send(schedule.encode())
    
    c_socket.send('==================\n'.encode())  

    time.sleep(3)
    
    print("Waiting for RFID/NFC card...")

c.close()
