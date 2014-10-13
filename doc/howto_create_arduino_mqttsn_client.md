# How to create arduino MQTT-SN client

first go to Arduino website and download arduino software for your operating system:
[Arduino](http://arduino.cc/en/Main/Software)

then you need to clone this git repo:

```
cd ~
git clone git@github.com:zadata/mqtt-sn-arduino.git
```
now we need to create a symbolic links from downloaded repository to arduino libraries folder:
	
```
cd ~/PATH_TO_ARDUINO/Arduino/libraries
ln -s ~/ws/mqtt-sn-arduino/arduino/MQTTSN/ .
ln -s ~/ws/mqtt-sn-arduino/arduino/XBee/ .
ln -s ~/ws/mqtt-sn-arduino/arduino/MemoryFree/ .
```
now go to MQTTSN/sketches library and open mqttsn_client.ino using Arduino IDE software, then connect arduino to your pc using usb cable and upload the sketch.  
 

Lets built together step by step the arduino client.

In this tutorial we will build an arduino client that will publish temperature and light level measurement using MQTTS protocol.  
Also the client will have a controled led that will controled using mqtt protocol. 

Parts necessary for performing the task:

1. [SparkFun Inventor's Kit for Arduino](https://www.sparkfun.com/products/11227)
2. [XBee Shield](https://www.sparkfun.com/products/10854)
3. [Arduino Stackable Header - 6 Pin](https://www.sparkfun.com/products/9280?)
4. [Arduino Stackable Header - 8 Pin](https://www.sparkfun.com/products/9279?)
5. [XBee 2mW Wire Antenna - Series 2 (ZigBee Mesh)](https://www.sparkfun.com/products/10414)

First, place the Arduino socket located at SparkFun Inventor's Kit,
 
![todo](https://github.com/zadata/mqtt-sn-arduino/raw/master/doc/images/mqttsn_client_0.jpg "todo")
![todo](https://github.com/zadata/mqtt-sn-arduino/raw/master/doc/images/mqttsn_client_1.jpg "todo")

Now we will solder the pins to the XBee Shield and get the following result

![todo](https://github.com/zadata/mqtt-sn-arduino/raw/master/doc/images/mqttsn_client_2.jpg "todo")

After we configure the antenna according to the following guide:  
[howto configure xbee firmware](https://github.com/zadata/mqtt-sn-arduino/blob/master/doc/howto_configure_xbee_firmware.md) we place it on the XBee Shield and get:

![todo](https://github.com/zadata/mqtt-sn-arduino/raw/master/doc/images/mqttsn_client_3.jpg "todo")

Now we connect the XBee Shield to Arduino and make sure that SPDT switch on xbee shield is on DLINE because in our mqttsn_client sketch xbee antenna connected to software serial pins 2,3.

![todo](https://github.com/zadata/mqtt-sn-arduino/raw/master/doc/images/mqttsn_client_4.jpg "todo")

Now we start building the components are on the breadboard.  
first we make an overview of all the components we use:  
There are eight LEDs in use. we will explain what signifies all the LEDs (the transition is from bottom to top):  
- First LED (yellow) signifies that the client sent a request to send a message to its XBee antenna.  
- Second LED (yellow) signifies that the client received ack from its XBee antenna on the sending request he had initiated.  
- Third LED (yellow) signifies that the client received a message intended for him.  
- Fourth LED (red) signifies that the client was able to connect to the gateway (part of MQTTS protocol).  
- Fifth LED (red) signifies that the client was able to connect to the broker (part of MQTT protocol).  
- Six LED (red) signifies that the client was able to make registration of these topics is going to publish.  
- Seven LED (red) signifies that there is a problem in the system.  
- Eight LED (yellow) is actually led him we rule, ie we turn on and off it proactively. 
 
All LEDs connected with one leg to ground thru 330 ohm resistor and with second leg to arduino.

Use [Mini Photocell](https://www.sparkfun.com/products/9088?) from 'SparkFun Inventor's Kit for Arduino' connect one leg of the Photocell to ground through 10K resistor, the same leg we'll hook also to arduino later. The second leg of the Photocell will be connected to the 5V comes from arduino.

The other sensor we used is a [TMP36 - Temperature Sensor](https://www.sparkfun.com/products/10988) from 'SparkFun Inventor's Kit for Arduino, this sensor has three legs, the right leg from flat side of the sensor plug to ground, plug left leg later to 5V comes from arduino and the middle leg we will plug later to analog input of arduino.

Also we will bridge between the two rails of the breadboard input for connection convenience


![todo](https://github.com/zadata/mqtt-sn-arduino/raw/master/doc/images/full_mqttsn_client_5.jpg "todo")

Now we make the connection between the breadboard to arduino.
There are a lot of connections that need to be done so you have to do it with patience and precision to maximize the results.

1. First plug wire between 3.3V port of arduino to the plus input of the track feed breadboard. 
2. Plug wire from the arduino ground port to minus input of the breadboard
3. Plug wire from arduino digital input number 7 to first LED.
4. Plug wire from arduino digital input number 8 to second LED.
5. Plug wire from arduino digital input number 9 to third LED.
6. Plug wire from arduino digital input number 10 to fourth LED.
7. Plug wire from arduino digital input number 11 to fifth LED.
8. Plug wire from arduino digital input number 12 to six LED.
9. Plug wire from arduino digital input number 13 to seven LED.
10. Plug wire from arduino digital input number 6 to eight LED.
11. Plug wire from arduino 5V output port to Photocell free leg and temperature sensor left leg (from flaten side).
12. Plug wire from arduino analog input number 0 to Photocell other (not conected to 5V) leg.
13. Plug wire from arduino analog input number 1 to temperature sensor middle leg.  


![todo](https://github.com/zadata/mqtt-sn-arduino/raw/master/doc/images/full_mqttsn_client_6.jpg "todo")  

#### Connection scheme  

![connection scheme](https://github.com/zadata/mqtt-sn-arduino/raw/master/doc/images/mqttsn_client_connection_scheme.jpg "todo")  
