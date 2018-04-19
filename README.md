# LTC2990
Raspberry Pi3 I2C driver for Analog Devices LTC2990 IC<br>
This is a power monitor IC - you can measure voltage, shunt current and remote temperature using a transistor. The binary is also included if you do not wish to compile. The dependencies are g++ and WiringPi both of which are included in the Raspbian distribution. I will provide a development board with header pins for breadboard if anyone is interested, contact me here or at career4KW@gmail.com
I plan to use my previous Raspberry PI Node web server to put the ltc2990 data on the web using the Pi as the node server.This will be included here when completed.

<b>TODO</b><br>
    * Implement timeout for busy bit <br>
    * ncurses display<br>
    * Node web server demo <i>See previous Node project: https://kwaite1021.github.io/piFrame/ </i> <br>
    * Add run option argument for time between measurements - currently this is set to approx. 2 seconds
  
<b>Features</b><br>
    * Good accuracy 14 bit ADC internal - easy to use no complicated ADC setups <br>
    * Could also add remote temperature using a voltage output device such as the AD22100 <br>
    * Excellent for measuring resistive current shunt voltages<br>
    * The console driver provided allows for logging of data to a csv file, with or without std output to screen<br>
    
<b>Raspberry Pi setup</b><br>
    Quick:<br>
    (1) You need to have I2C enabled on the pi via raspi-config <br>
    (2) Ensure WiringPi library is installed [Should be installed and ready to go with Raspbian] <br>
    (3) Wire your LTC2990 to your Raspberry Pi using I2C bus 0 <br>
    (4) Test your I2C setup with Wiring Pi [this should already be installed]. Use 'i2cdetect -y 1' <br>
    NOTE: I used the Pi address 0x4C as this is where my LTC2990 is mapped to - you will see your address when 
    you run the i2cdetect program as above. I have set my LTC2990 to the default hardware address 00 on the LTC2900 pins 8 &     9. If your address does not appear as 0x4C when the i2cdetect program is run, then you will need to modify this driver source. I am working to automatically update this address in the next release. The WiringPi library uses this address to talk to the device.Please do use the reference links below if you have issues.
     

<b>Driver usage</b><br>
    <p>  to compile: 'g++ ltc2990.cpp -lwiringPi -o console_ltc2990_v1' <br>
         to run: ./console_ltc2990_v1
         Running without any arguments lists V1, V2 & TR2 repeatedly, in the console. I have used the default setup where the            shunt voltage across the resistor is V1-V2, and TR2 is the remote temperature sensor.<br>
   ./console_ltc2990_v1 -h <i>This lists the run arguments.</i><br>
   ./console_ltc2990_v1 -l <i> Values shown in std output and logs to csv file.</i><br>
   ./console_ltc2990_v1 -s <i> Supress std output and log data to csv file.</i><br>
    
<b>Reference links: </b><br>
http://www.analog.com/en/products/monitor-control-protection/power-monitors/ltc2990.html<br>
http://wiringpi.com/<br>
http://wiringpi.com/reference/i2c-library/<br>
https://learn.sparkfun.com/tutorials/raspberry-pi-spi-and-i2c-tutorial<br>

Thanks,<br>
Kevin Waite
