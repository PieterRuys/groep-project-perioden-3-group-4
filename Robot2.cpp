#include "BrickPi3.h" // for BrickPi3
#include <iostream>      // for cin and cout
#include <unistd.h>     // for usleep
#include <signal.h>     // for catching exit signals

using namespace std;

BrickPi3 BP;

void exit_signal_handler(int signo);

uint16_t Light;
uint16_t Color;
sensor_light_t mylight;
sensor_color_t mycolor;

int16_t getlight(){
  BP.get_sensor(PORT_3, mylight);
  int16_t val = mylight.reflected;
  return val
}

int16_t getcolor(){
  BP.get_sensor(PORT_1, mycolor);
  uint16_t val = mycolor.reflected_red;
  return val
}

int main(){
  signal(SIGINT, exit_signal_handler); // register the exit function for Ctrl+C

  BP.detect(); // Make sure that the BrickPi3 is communicating and that the firmware is compatible with the drivers.

  BP.set_sensor_type(PORT_3, SENSOR_TYPE_NXT_LIGHT_ON);
  BP.set_sensor_type(PORT_1, SENSOR_TYPE_NXT_COLOR_RED);
  
  string regel;
  cout << "plaats sensor recht boven de lijn (zwart) en voer in a gevolgd door enter" << endl;
  cin >> regel;
  BP.get_sensor(PORT_3, mylight);
  BP.get_sensor(PORT_1, mycolor);
  Light = mylight.reflected;
  Color = mycolor.reflected_red;
  cout << "Light =" << Light << endl;
  cout << "Color =" << Color << endl;
  
  int16_t lightval;
  int16_t colorval;
  int16_t power = 20;
  
  while(true){
    lightval = getlight();
    colorval = getcolor();
    cout << lightval << endl;
    cout << colorval << endl;
    if ((lightval - light) > 50){
      BP.set_motor_power(PORT_B, -10);
      BP.set_motor_power(PORT_C, 40);
    }
    if ((color - colorval) > 100{
      BP.set_motor_power(PORT_B, 40);
      BP.set_motor_power(PORT_C, -10);
    }
    //else{
      //BP.set_motor_power(PORT_B, 30);
      //BP.set_motor_power(PORT_C, 30);
    //}
    usleep(1000000);
 }   
  
}
  
  
 void exit_signal_handler(int signo){
  if(signo == SIGINT){
    BP.reset_all();    // Reset everything so there are no run-away motors
    BP.set_sensor_type(PORT_1, SENSOR_TYPE_NONE);  //Doesn't work, sorry.
    exit(-2);
  }
}
