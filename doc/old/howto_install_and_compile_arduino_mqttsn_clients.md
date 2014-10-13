1. Create a symlink to MQTT-SN library from git repo inside arduino/libraries folder:

``` bash
cd ~/arduino-1.0.1/libraries
mv MQTTSN/ MQTTSN_bak
ln -s ~/ws/mqtt-sn-arduino/arduino/MQTTSN/ .
ln -s ~/ws/mqtt-sn-arduino/arduino/XBee/ .
ln -s ~/ws/mqtt-sn-arduino/arduino/MemoryFree/ .
```

2. Import library in Arduino IDE

3. On Gateway machine install XBee explorer USB driver for your OS (choose D2XX):

http://www.ftdichip.com/FTDrivers.htm
