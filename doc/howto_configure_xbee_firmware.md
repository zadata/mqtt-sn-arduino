# XBee firmware configuration procedure

XBee modems can be configured in 3 different configurations:
- One ZigBee Coordinator
- One or several ZigBee Routers
- One or several ZigBee End Devices

First install FTDI USB drivers as described here:
- [howto_install_FTDI_drivers](https://github.com/zadata/mqtt-sn-arduino/blob/master/doc/howto_install_FTDI_drivers.md)

The configuration done using X-CTU Windows program.
If you don't have Windows you will need wine or Windows VM.

### Windows

1. Download XCTU installer from:
- http://ftp1.digi.com/support/utilities/40003002_B.exe
2. Run XCTU installer


### Linux

See page 33 in "Building Wireless Sensor Networks" (BWSN) book.

To use X-CTU under Linux, you’ll need to first install Wine, which lets you run Windows applications 
under the X Window System. On a Linux system, you can usually install Wine using your Linux package manager.
Next, using Wine, download the X-CTU installer and run this command:

``` bash
wine 40002637_c.exe
```

(If the filename is not 40002637_c.exe, replace it with the name of the file you downloaded.)
Now create a symbolic link between the serial port that corresponds to your XBee and a Windows serial port, 
such as `COM10`:

``` bash
ln -s /dev/ttyUSB0 ~/.wine/dosdevices/com10
```

The actual device filename (`ttyUSB0` in the example) will vary, so look at the output of the `dmesg` command shortly after you plug in the XBee adapter to see which device was added.
Next, launch X-CTU using a command something like:

``` bash
wine .wine/drive_c/Program\ Files/Digi/XCTU/X-CTU.exe
```

Click the `User Com Ports` tab and type in the name of the Com port you created (such as `COM10`), then click Add. Unfortunately, you will need to do this each time you launch X-CTU, as it does not remember the custom Com ports.


### OS-X
I used the following guide:
[run X-CTU on mac](http://sentman.com/users/james/weblog/bd1c5)

1. Install `wine` to run the XCTU software on OSX operating system. 
use [Installing Wine on Mac OS X](http://www.davidbaumgold.com/tutorials/wine-mac).

2. download XCTU installer [link](http://ftp1.digi.com/support/utilities/40003002_B.exe)

3. run XCTU installer using wine
``` bash
cd ~/Downloads
wine 40003002_B.exe
```

4. create a symbolic link to the serial port in your /dev directory inside of the ~/.wine/dosdevices directory
``` bash
ln -s /dev/tty.usbserial-A100RWT1 ~/.wine/dosdevices/com1
```

5. run XCTU using wine
``` bash
cd ~/.wine/drive_c/Program\ Files/Digi/XCTU
wine X-CTU.exe
```


## XBee firmware configurations

For each XBee modem:  
  1. Insert XBee modem into XBee Explorer USB dongle  
  2. Connect the XBee Explorer dongle to USB  
![xbee usb explorer](https://github.com/zadata/mqtt-sn-arduino/raw/master/doc/images/XCTU_screen_shots/XBEE_USB_EXPLORERE.png "todo")  
  3. go to "User Com Ports" tab  
![XCTU user com ports](https://github.com/zadata/mqtt-sn-arduino/raw/master/doc/images/XCTU_screen_shots/XCTU-1.png "todo")  
  4. add "COM1" user  
![XCTU user com ports](https://github.com/zadata/mqtt-sn-arduino/raw/master/doc/images/XCTU_screen_shots/XCTU-2.png "todo")  
  5. select COM1 in "select com port" windows click on Test Query button to check the connection  
![XCTU conectivity check](https://github.com/zadata/mqtt-sn-arduino/raw/master/doc/images/XCTU_screen_shots/XCTU-3.png "todo")  
  6. You should see a message with the details of the communications antenna  
![conectivity message](https://github.com/zadata/mqtt-sn-arduino/raw/master/doc/images/XCTU_screen_shots/XCTU-4.png "todo")    
  7. select "Modem Configuration" tab and click on "Read" button  
![Modem configuretion tab](https://github.com/zadata/mqtt-sn-arduino/raw/master/doc/images/XCTU_screen_shots/XCTU-5.png "todo")  
  8. You need to select different values depending on the role od XBee modem:  
    - for Coordinator: in "Modem:Xbee window" choose `XB24-ZB`, in "function Set" window choose `ZIGBEE COORDINATOR API` and in "Version" window chhose the last version.  
    ![coordinator](https://github.com/zadata/mqtt-sn-arduino/raw/master/doc/images/XCTU_screen_shots/XCTU-6.png "todo")  
    - for Router: in "Modem:Xbee window" choose `XB24-ZB`, in "function Set" window choose `ZIGBEE ROUTER API` and in "Version" window chhose the last version.  
    ![router](https://github.com/zadata/mqtt-sn-arduino/raw/master/doc/images/XCTU_screen_shots/XCTU-7.png "todo")  
    - for End-Device: in "Modem:Xbee window" choose `XB24-ZB`, in "function Set" window choose `ZIGBEE END DEVICE API` and in "Version" window chhose the last version.  
    ![end device](https://github.com/zadata/mqtt-sn-arduino/raw/master/doc/images/XCTU_screen_shots/XCTU-8.png "todo")  
  9. choose PAN ID (It should be the same in all modules that make up the network)  
  10. choose parameters then click on "Write" button  

