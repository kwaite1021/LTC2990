# LTC2990
Raspberry Pi3 I2C driver for Analog Devices LTC2990 IC<br>
<img src= "LTC2990.png"> <br>
This is a power monitor IC - you can measure voltage, shunt current and remote temperature using a transistor. The binary is also included if you do not wish to compile. The dependencies are g++ and WiringPi both of which are included in the Raspbian distribution. I will provide a development board with header pins for breadboard if anyone is interested, contact me here or at career4KW@gmail.com<br>
NOTE: Analog Devices has the Linduino platform and also provides drivers for many devices, check here: <br>
https://github.com/analogdevicesinc/Linduino <br>
and here is the Analog devices home page for the LTC2990: http://www.analog.com/en/products/monitor-control-protection/power-monitors/ltc2990.html#product-overview <br>
I plan to use my previous Raspberry PI Node web server to put the ltc2990 data on the web using the Pi as the node server.This will be included here when completed. <br>
<b> Updated 13MAY2018 </b><br>
    * Streamlined code <br>
    * Implemented error codes and timeout for conversion <br>
   
<b>TODO</b><br>
    * Node web server demo <i>See previous Node project: https://kwaite1021.github.io/piFrame/ </i> <br>    
  
<b>Features</b><br>
    * Very good accuracy 14 bit ADC internal - easy to use no complicated ADC setups <br>
    * Could also add separate remote temperature using a voltage output device such as the AD22100 <br>
    * Excellent for measuring resistive current shunt voltages<br>
    * The console driver provided allows for logging of data to a csv file, with or without std output to screen<br>
    
<b>Raspberry Pi setup</b><br>
    Quick:<br>
    (1) You need to have I2C enabled on the pi via raspi-config <br>
    (2) Ensure WiringPi library is installed [Should be installed and ready to go with Raspbian] <br>
    (3) Wire your LTC2990 to your Raspberry Pi using I2C bus 1 <br>
    (4) For the web server, you need to have Node / npm setup  <br>  
     

<b>Driver usage standalone</b><br>
    <p>  to compile: 'g++ ltc2990.cpp -lwiringPi -o console_ltc2990_v1' <br>
         to run: ./NodeLTC2990
         Running without any arguments runs once in the console. I have used the default setup where the shunt voltage across the resistor is V1-V2, and TR2 is the remote temperature sensor.<br>
   ./NodeLTC2990 -h <i>This shows help then exits</i><br>
   Help for NodeLTC2990 v1.1 by Kevin Waite <br>
  Run without arguments for normal usage <br>
  Output format: voltage V2,current in amps,temperature TR2, errors <br>
 -s Show table of i2c devices on bus 1 <br>
  Error codes: <br>
  Error = 0 no errors found <br>
  Error = 1 Temperature sensor open <br>
  Error = 2 Temperature sensor shorted <br>
  Error = 3 No response from device <br>
  </p>   
  Running with -s option <br>
  <img src= "i2c_table.png"> <br>
   
    
<b>Reference links: </b><br>
http://www.analog.com/en/products/monitor-control-protection/power-monitors/ltc2990.html<br>
http://wiringpi.com/<br>
http://wiringpi.com/reference/i2c-library/<br>
https://learn.sparkfun.com/tutorials/raspberry-pi-spi-and-i2c-tutorial<br>

<p>
NOTE: I used the Pi address 0x4C as this is where my LTC2990 is mapped to - you will see your address when 
      you run the i2cdetect program as above. I have set my LTC2990 to the default hardware address 00 on the LTC2900 pins 8       & 9. If your address does not appear as 0x4C when the i2cdetect program is run, then you will need to modify this             driver source. I am working to automatically update this address in the next release. The WiringPi library uses this         address to talk to the device.Please do use the reference links below if you have issues.
</p>

Thanks,<br>
Kevin Waite
