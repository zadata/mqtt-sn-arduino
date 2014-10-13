
TODO: Nike+ demo
# How to create arduino nike plus without xbee shield

first go to Arduino website and download arduino software for your operating system:
[Arduino](http://arduino.cc/en/Main/Software)

then you need to clone this repo:

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
ln -s ~/ws/mqtt-sn-arduino/arduino/public_nike_dec/ .
```
now go to MQTTSN/sketches library and open mqttsn_client_nike_plus.ino using Arduino IDE software, then connect arduino to your pc using usb cable and upload the sketch.  
 

Lets built together step by step the arduino client.

In this tutorial we will build an arduino client that will publish temperature and light level measurement using MQTTS protocol.  
Also the client will have a controled led that will controled using mqtt protocol. 

Parts necessary for performing the task:

1. [SparkFun Inventor's Kit for Arduino](https://www.sparkfun.com/products/11227)
2. [Basic Breadboard](https://www.sparkfun.com/products/112)
3. [Breakout Board for XBee Module](https://www.sparkfun.com/products/8276)
4. [Break Away Headers - Straight](https://www.sparkfun.com/products/116)
5. [2mm 10pin XBee Socket](https://www.sparkfun.com/products/8272?)  
5. [XBee 2mW Wire Antenna - Series 2 (ZigBee Mesh)](https://www.sparkfun.com/products/10414)  
6. [Nike+iPod Serial to USB Adapter](https://www.sparkfun.com/products/8245)  
7. [Nike+ Stand Alone Sensor Kit](http://www.amazon.com/Nike-Stand-Alone-Sensor-Kit/dp/B001L6LJJS)  

First, we will prepare the [Basic Breadboard](https://www.sparkfun.com/products/112)  
 
![todo](https://github.com/zadata/mqtt-sn-arduino/raw/master/doc/images/mqttsn_client_without_xbee_shield_0.jpg "todo")  

Then we will solder the [Break Away Headers - Straight](https://www.sparkfun.com/products/116) and [2mm 10pin XBee Socket](https://www.sparkfun.com/products/8272?) to [Breakout Board for XBee Module](https://www.sparkfun.com/products/8276) and get the following result
Now we will solder the pins to the XBee Shield and get the following result

![todo](https://github.com/zadata/mqtt-sn-arduino/raw/master/doc/images/mqttsn_client_without_xbee_shield_1.jpg "todo")

now we will attach the Breakout Board for XBee Module to the breadboard and plug pin 1 of Breakout Board for XBee Module to breadbord '+' track and pin 10 of Breakout Board for XBee Module to breadboard '-' track.  

![todo](https://github.com/zadata/mqtt-sn-arduino/raw/master/doc/images/mqttsn_client_without_xbee_shield_2.jpg "todo")

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

![todo](https://github.com/zadata/mqtt-sn-arduino/raw/master/doc/images/mqttsn_client_without_xbee_shield_3.jpg "todo")  
![todo](https://github.com/zadata/mqtt-sn-arduino/raw/master/doc/images/mqttsn_client_without_xbee_shield_4.jpg "todo")  


After we configure the antenna according to the following guide:  
[howto configure xbee firmware](https://github.com/zadata/mqtt-sn-arduino/blob/master/doc/howto_configure_xbee_firmware.md) we place it on the XBee Shield and get:  

![todo](https://github.com/zadata/mqtt-sn-arduino/raw/master/doc/images/mqttsn_client_without_xbee_shield_5.jpg "todo")  

now we will prepare the nike plus components and connect them to breadboard:  
![todo](https://github.com/zadata/mqtt-sn-arduino/raw/master/doc/images/nike_plus_demo_0.jpg "todo")  

First it is necessary to solder the [Break Away Headers](https://www.sparkfun.com/products/116) to [Nike+iPod Serial to USB Adapter](https://www.sparkfun.com/products/8245)   
![todo](https://github.com/zadata/mqtt-sn-arduino/raw/master/doc/images/nike_plus_demo_1.jpg "todo") 

connect the [Nike+iPod Serial to USB Adapter](https://www.sparkfun.com/products/8245) to the breadboard and get the following result:
![todo](https://github.com/zadata/mqtt-sn-arduino/raw/master/doc/images/nike_plus_demo_2.jpg "todo")   

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
14. Plug wire from arduino digital input number 0 (RX) to Breakout Board for XBee Module pin number 2.  
15. Plug wire from arduino digital input number 1 (TX) to Breakout Board for XBee Module pin number 3.
16. Plug wire from arduino digital input number 2 to Rx pin of Nike+ serial adpter.
17. Plug wire from arduino digital input number 1 to Tx pin of Nike+ serial adpter.    


![todo](https://github.com/zadata/mqtt-sn-arduino/raw/master/doc/images/nike_plus_demo_3.jpg "todo")  


#### Connection scheme  

![connection scheme](https://github.com/zadata/mqtt-sn-arduino/raw/master/doc/images/mqttsn_client_nike+_connection_scheme.jpg "todo")  
