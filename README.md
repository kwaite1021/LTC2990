# LTC2990
Raspberry Pi3 I2C driver for Analog Devices LTC2990 IC
This is a power monitor IC - you can measure voltage, shunt current and remote temperature using a transistor. The binary is also included if you do not wish to compile. The dependencies are g++ and WiringPi both of which are included in the Raspbian distribution. I will provide a development board with header pins for breadboard if anyone is interested, contact me here or at career4KW@gmail.com
I plan to use my previous Raspberry PI Node web server to put the ltc2990 data on the web using the Pi as the node server.This will be included here when completed.

<b>TODO</b><br>
    * Implement timeout for busy bit <br>
    * ncurses display<br>
    * Node web page demo<br>
  
<b>Features</b><br>
    * Good accuracy 14 bit ADC internal - easy to use no complicated ADC setups
    * Could also add remote temperature using a voltage output device such as the AD22100 <br>
    * Excellent for measuring resistive current shunt voltages<br>
    
<b>Raspberry Pi setup</b><br>
    * Read the project file "Pi setup for I2C"<br>
    
<b>Reference links: </b><br>
http://www.analog.com/en/products/monitor-control-protection/power-monitors/ltc2990.html<br>
http://wiringpi.com/<br>
http://wiringpi.com/reference/i2c-library/<br>
https://learn.sparkfun.com/tutorials/raspberry-pi-spi-and-i2c-tutorial<br>

Thanks,
Kevin Waite
