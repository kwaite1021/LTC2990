/*  Driver for LTC2990 with Raspberry Pi 3
    by Kevin Waite 13MAY2018
    No repeated measuremets, trigger for each acquisition
    http://www.analog.com/en/products/monitor-control-protection/power-monitors/ltc2990.html
    http://wiringpi.com/reference/i2c-library/
    https://learn.sparkfun.com/tutorials/raspberry-pi-spi-and-i2c-tutorial
    linduino c++ code: http://www.analog.com/media/en/simulation-models/linduino/LTC2990.cpp
    wiringPi I2C library: http://wiringpi.com/reference/i2c-library/
    Useful commands:
    i2cdetect -y 1
    This will probe all the addresses on bus 1, and report whether
    any devices are present.
    This program assumes LTC2990 setup as address 0 [pins 8& 9]
    To build:
    g++ NodeLTC2990.cpp -lwiringPi  -o NodeLTC2990

*/
#include <stdio.h>
#include <iostream>
//#include <fstream>
#include <string>
#include <errno.h>
#include <wiringPiI2C.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <cmath>
#include <iomanip>
#include <chrono>
#include <thread>


using namespace std;
int stat29 = 0x00;    //Status register
uint8_t MSG2990 = 0;
int result = 0;
static float data29[4];
/* Messages to server:
    MSG2990 = 00000000     no errors
    MSG2990 = 00000001     Sensor open
    MSG2990 = 00000010     Sensor shorted
    MSG2990 = 00000011     No response from device
*/
// Initialize the i2c interface by giving it an external device ID,
// and return file descriptor
int fd = wiringPiI2CSetup(0x4C);
void Trigger(){
  // Trigger one measurement of all parameters
  result = wiringPiI2CWriteReg8 (fd,0x02,0x01);
}

//Function checks for response from device
bool  CheckBusyResponse(){
  bool BusyBit = true, Tr = 0;
  for( int Tr = 1; Tr < 100; Tr ++) {
    uint8_t BB = wiringPiI2CReadReg8(fd,stat29);
    BusyBit = (BB & 0x01);
    //cout << "BusyBit: " << BusyBit << endl;
      if (!BusyBit){break;}
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    if(BusyBit){
      MSG2990 = 0x03;
    }
  return BusyBit;
}

uint16_t GetVoltage(int8_t rMSB, int8_t rLSB){
  uint8_t VMSB = wiringPiI2CReadReg8(fd,rMSB);
  uint8_t VLSB = wiringPiI2CReadReg8(fd,rLSB);
  // data not containing valid and sign bits
  VMSB &= 0x3F;
  uint16_t dWord = (VMSB << 8) | (VLSB & 0xff);
  return dWord;
}

bool dValid(uint8_t tREG){
  bool rVal = false;
  //Read the status register
  uint8_t vRES = wiringPiI2CReadReg8(fd,stat29);
  switch (tREG) {
    case 0x04:  //V1
      {
        rVal = (vRES & 0x04) >> 2;
      }
    case 0x08:  //V2
      {
        rVal = (vRES & 0x08)>>3;
      }
    case 0x10:  //TR2
      {
        rVal = (vRES & 0x10)>>4;
      }
  }
  return rVal;
}

void GetData(){
  float V1 = 0, V2 = 0, ShuntV = 0, TR2V = 0;
  bool V1valid = false, V2valid = false, TR2valid = false;
  bool SENSopen = false, SENSshort = false;
  int16_t VWord  = 0;
  uint8_t sign = 1;
  // Check for new data V1
  V1valid = dValid(0x04);
  // Check for new data V2
  V2valid = dValid(0x08);
  // Check for new data TR2 [Temperature]
  TR2valid = dValid(0x10);
  // Get data from registers
  // Voltage on V1
  if(V1valid){
      VWord = GetVoltage(0x06,0x07);
      V1 = VWord * 305.18e-6;
      V1 = roundf(V1 * 1000) / 1000;
    }else{
      V1 = 0;
    }

  // Voltage on V2
  if(V2valid){
      VWord = GetVoltage(0x08,0x09);
      V2 = VWord * 305.18e-6;
      V2 = roundf(V2 * 1000)/1000;
    }else{
      V2 = 0;
    }
    // Temperature
    VWord = GetVoltage(0x0A,0x0B);
    // Sensor open?
    SENSopen = (VWord & 0x2000) >> 13 ;
    // Sensor shorted?
    SENSshort = (VWord & 0x4000) >> 14 ;
    if(TR2valid && !SENSopen && !SENSshort){
        // data not containing valid and sensor bits
        VWord &= 0x1FFF;
        //Check the sign bit
        if ((VWord & 0x1000)>>12){
          sign = -1;
          VWord = (VWord ^ 0x7FFF) + 1;
        }
        TR2V = VWord * 0.0625 * sign;
        TR2V = roundf(TR2V * 10)/10;

      }else{
        if(SENSopen){
          // cout << "Temperature: Sensor reads OPEN " << "\n";}
          MSG2990 |= 0x01;
          TR2V = 0;
        }
        if(SENSshort){
          // cout << "Temperature: Sensor SHORT " << "\n";}
          MSG2990 |= 0x02;
          TR2V = 0;
       }
     }
      ShuntV = (V1 - V2) / 0.1;
      ShuntV = roundf(ShuntV * 1000)/1000;
      data29[0] = V2;
      data29[1] = ShuntV;
      data29[2] = TR2V;
      data29[3] = MSG2990;
}

void display_help(){
  cout << "Help for NodeLTC2990 v1.1 by Kevin Waite" << "\n";
  cout << "Run without arguments for normal usage" << "\n";
  cout << "Output format: \'voltage V2,current in amps,temperature TR2, errors\'" << "\n";
  cout << "-h Show help message and exit" << "\n";
  cout << "-s Show table of i2c devices on bus 1" << "\n";
  cout << "Error codes:" << "\n";
  cout << "Error = 0 no errors found" << "\n";
  cout << "Error = 1 Temperature sensor open" << "\n";
  cout << "Error = 2 Temperature sensor shorted" << "\n";
  cout << "Error = 3 No response from device" << "\n";

}
void show_table(){
  system("sudo i2cdetect -y 1");
}

int main(int argc, char** argv) {
  int16_t sign = 1;
  int opt, tCount = 0;
  string input = "";
  bool flagH = false, flagS = false;
  float voltsV1,currentI1,Temp1 = 0;
  while ( (opt = getopt(argc, argv, "h,s")) != -1 ) {  // for each option...
        switch ( opt ) {
            case 'h':   //Display Help and exit
              {
                flagH = true;
                display_help();
                return 0;
                break;
              }
              case 's':   //Display Help and exit
                {
                  flagS = true;
                  show_table();
                  return 0;
                  break;
                }
            case '?':  // unknown option...
              {
                    cerr << "Unknown option: '" << char(optopt) << "'!" << endl;
                break;
              }
        }
    }

    // Write control reg for reading all parameters 01111000
    result = wiringPiI2CWriteReg8 (fd,0x01,0x78);
    Trigger();
    bool BB = CheckBusyResponse();
    if(!BB){
        //cout << "Response OK: " << BB << endl;
        GetData();
        cout << data29[0] << "," << data29[1] << "," << data29[2] << "," << data29[3] << '\n';
      }else{ //No response from device
        data29[0] = 0;
        data29[1] = 0;
        data29[2] = 0;
        data29[3] = MSG2990;
        //cout << "No response : " << BB << endl;
        cout << data29[0] << "," << data29[1] << "," << data29[2] << "," << data29[3] << '\n';
        }
  return 0;
}
