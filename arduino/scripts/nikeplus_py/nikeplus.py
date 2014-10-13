#!/usr/bin/env python

# See:
#   Nike+ Hacking with Python
#   Published on 18 May 2010 
# http://smus.com/nike-hacking-with-python/
# https://raw.github.com/borismus/Running-Gestures/master/hexbyte.py

import serial
from hexbyte import *

def readbytes(number):
    buf = ''
    for i in range(number):
        byte = ser.read()
        buf += byte

    return buf

# open the appropriate serial port
Device = '/dev/ttyUSB3'
#Device = '/dev/tty.usbserial-A6007uDh'

ser = serial.Serial(Device, 57600, bytesize=serial.EIGHTBITS)

# send the following init string to the Nike+ device:
init1 = 'FF 55 04 09 07 00 25 C7'
ser.write(HexToByte(init1))

# listen for the response string: FF 55 04 09 00 00 07 EC
response1 = ByteToHex(readbytes(8))
assert response1 == 'FF 55 04 09 00 00 07 EC'

# send the second init string
init2 = 'FF 55 02 09 05 F0'
ser.write(HexToByte(init2))

# listen for the response string: FF 55 04 09 06 00 25 C8
response2 = ByteToHex(readbytes(8))
assert response2 == 'FF 55 04 09 06 00 25 C8'

# now we're ready to listen for actual data
print "nike+ initialized. listening for data"

while True:
    byte = ser.read()
    # if a byte is coming down the port,
    if byte:
        # get the rest of the message (34 chars)
        message = byte + readbytes(33)
        # and decipher it
        data = {
            'number': ByteToHex(message[11]),
            'uid': ByteToHex(message[7:11]),
            'data': ByteToHex(message[12:]),
        }
        print data
