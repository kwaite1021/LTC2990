/*  Driver for LTC2990 with Raspberry Pi 3
    by Kevin Waite 12APR2018
    Ver 0.1 no repeated measuremets, trigger for each acquisition
    http://www.analog.com/en/products/monitor-control-protection/power-monitors/ltc2990.html
    http://wiringpi.com/reference/i2c-library/
    https://learn.sparkfun.com/tutorials/raspberry-pi-spi-and-i2c-tutorial
    linduino c++ code: http://www.analog.com/media/en/simulation-models/linduino/LTC2990.cpp
    wiringPi I2C library: http://wiringpi.com/reference/i2c-library/
    Useful commands:
    i2cdetect -y 1
    This will probe all the addresses on bus 1, and report whether
    any devices are present.
    To build:
    g++ ltc2990.cpp -lwiringPi -o console_ltc2990


*/
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <errno.h>
#include <wiringPiI2C.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <cmath>
#include <iomanip>

using namespace std;

int result = 0;
int stat29 = 0x00;    //Status register
void Sleep(float s)
{
    int sec = int(s*1000000);
    usleep(sec);
}
// Initialize the i2c interface by giving it an external device ID,
// and return file descriptor
int fd = wiringPiI2CSetup(0x4C);
void Trigger(){
  // Trigger one measurement of all parameters
  result = wiringPiI2CWriteReg8 (fd,0x02,0x01);
}
float * GetData(){
  float V1 = 0, V2 = 0, ShuntV = 0, V2rnd = 0, TR2V = 0;
  unsigned short int BusyBit = 1, VWord  = 0, VMSB = 0 , VLSB = 0 , V1ready = 0;
  unsigned short int V2ready = 0, TR2ready = 0;
  unsigned short int SENSopen = 0, SENSshort = 0, TMSB = 0, sign = 1;
  static float data29[3];
  // Wait for Busy bit
  while (BusyBit){
    stat29 = wiringPiI2CReadReg8(fd,0x00);
    BusyBit = (stat29 & 0x01);
    }
  // Check for new data V1
  stat29 = wiringPiI2CReadReg8(fd,0x00);
  V1ready = (stat29 & 0x04)>>2;
  // Check for new data V2
  stat29 = wiringPiI2CReadReg8(fd,0x00);
  V2ready = (stat29 & 0x08)>>3;
  // Check for new data TR2 [Temperature]
  stat29 = wiringPiI2CReadReg8(fd,0x00);
  TR2ready = (stat29 & 0x10)>>4;
  // Get data from registers
  // Voltage on V1
  if(V1ready){
      VMSB = wiringPiI2CReadReg8(fd,0x06);
      VLSB = wiringPiI2CReadReg8(fd,0x07);
      // data not containing valid and sign bits
      VMSB &= 0x3F;
      VWord = (VMSB << 8) | (VLSB & 0xff);
      V1 = VWord * 305.18e-6;
      V1 = roundf(V1 * 1000) / 1000;
    }else{
      V1 = 0;
    }

  // Voltage on V2
  if(V2ready){
      VMSB = wiringPiI2CReadReg8(fd,0x08);
      VLSB = wiringPiI2CReadReg8(fd,0x09);
      // data not containing valid and sign bits
      VMSB &= 0x3F;
      VWord = (VMSB << 8) | (VLSB & 0xff);
      V2 = VWord * 305.18e-6;
      V2rnd = roundf(V2 * 1000)/1000;
    }else{
      V2 = 0;
    }
    // Temperature
    VMSB = wiringPiI2CReadReg8(fd,0x0A);
    // Sensor open?
    SENSopen = (VMSB & 0x20) >> 5;
    // Sensor shorted?
    SENSshort = (VMSB & 0x40) >> 6;
    if(TR2ready && !SENSopen && !SENSshort){
        VLSB = wiringPiI2CReadReg8(fd,0x0B);
        // data not containing valid and sign bits
        VMSB &= 0x0F;
        VWord = (VMSB << 8) | (VLSB & 0xFF);
        if (TMSB & 0x10){
          sign = -1;
          VWord = (VWord ^ 0x7FFF) + 1;
        }
        TR2V = VWord * 0.0625 * sign;
        TR2V = roundf(TR2V * 10)/10;

      }else{
        if(SENSopen){cout << "Temperature: Sensor reads OPEN " << "\n";}
        if(SENSshort){cout << "Temperature: Sensor SHORT " << "\n";}
        TR2V = 0;
      }
      ShuntV = (V1 - V2) / 0.1;
      // cout << "V1 =  " << V1 << "\n";
      // cout << "V2 =  " << V2 << "\n";
      // cout << "Shunt V =  " << ShuntV << "\n";
      ShuntV = roundf(ShuntV * 1000)/1000;
      data29[0] = V2rnd;
      data29[1] = ShuntV;
      data29[2] = TR2V;
      return data29;
  }

void display_help(){
  cout << "Help for console_ltc2990 v1 by Kevin Waite" << "\n";
  cout << "-h Show help message and exit" << "\n";
  cout << "-l log measurements to csv file" << "\n";
  cout << "-s supress std out but log measurements" << "\n";
  cout << "Running without options outputs to console without logging" << "\n";
}

int main(int argc, char** argv) {
  int16_t sign = 1;
  float *MyData;
  int opt, tCount = 0;
  string input = "";
  bool flagH = false, flagL = false, flagS = false;
  std::ofstream lt2990CSV;
  float voltsV1,currentI1,Temp1 = 0;
  cout << "Initializing ltc2990 on bus 1:" << fd <<"\n";
  // Retrieve the (non-option) argument:
    // if ( (argc <= 1) || (argv[argc-1] == NULL) || (argv[argc-1][0] == '-') ) {  // there is NO input...
    //     cerr << "No argument provided!" << endl;
    //     //return 1;
    //     cout << "argc = " << argc <<"\n";
    //     cout << "argv = " << argv[argc-1] <<"\n";
    //     cout << "opt = " << argv[argc-1][0] <<"\n";
    // }
    // else {  // there is an input...
    //     input = argv[argc-1];
    // }
    // Retrieve the options:

  while ( (opt = getopt(argc, argv, "hls")) != -1 ) {  // for each option...
        switch ( opt ) {
            case 'h':   //Display Help and exit
              {
                flagH = true;
                display_help();
                return 0;
                break;
              }

            case 'l':   //Log output to file
              {
                flagL = true;
                cout << "Logging data to power.csv" <<"\n";
                //Create / open CSV file
                lt2990CSV.open("power.csv");
                break;
              }
            case 's':   //Supress std out , log data
              {
                flagL = true;
                flagS = true;
                cout << "Logging data to power.csv, stealth mode" <<"\n";
                //Create / open CSV file
                lt2990CSV.open("power.csv");
                break;
              }

            case '?':  // unknown option...
              {
                    cerr << "Unknown option: '" << char(optopt) << "'!" << endl;
                break;
              }
        }
    }

  while(1){
    // Write control reg for reading all parameters 01111000
    result = wiringPiI2CWriteReg8 (fd,0x01,0x78);
    // Trigger one read of data approx every two seconds
    Trigger();
    MyData = GetData();
    if (!flagS){
      cout << MyData[0] << " Volts" << '\n';
      cout << MyData[1] << " Amps" << '\n';
      cout << MyData[2] << " deg C" << '\n';
      cout << "***********" << '\n';
    }
    if(flagL | flagS){
       lt2990CSV << tCount << "," << MyData[0] << "," <<  MyData[1] << "," << MyData[2] << endl;
     }
    Sleep(2);
    tCount ++;
  }
   return 0;
}
