#include "BrickPi3.h" // for BrickPi3
#include <iostream>      // for cin and cout
#include <unistd.h>     // for usleep
#include <signal.h>     // for catching exit signals

using namespace std;

BrickPi3 BP;

void exit_signal_handler(int signo);


uint16_t MIN_LIGHT;
uint16_t MAX_LIGHT;
uint16_t MIN_RGB;
uint16_t MAX_RGB;
sensor_color_t mycolor;
sensor_light_t mylight;

int16_t getRGB(){
  BP.get_sensor(PORT_1, mycolor);
  uint16_t val = mycolor.reflected_blue;
  if (val < MIN_RGB) val = MIN_RGB;
  if (val > MAX_RGB) val = MAX_RGB;
  return (100*(val - MIN_RGB))/(MAX_RGB - MIN_RGB);
}

int16_t getlight(){
  BP.get_sensor(PORT_3, mylight);
  uint16_t val = mylight.reflected;
  if (val > MIN_LIGHT) val = MIN_LIGHT;
  if (val < MAX_LIGHT) val = MAX_LIGHT;
  return (100*(val - MAX_LIGHT))/(MIN_LIGHT - MAX_LIGHT);
}

int main(){
  signal(SIGINT, exit_signal_handler); // register the exit function for Ctrl+C

  BP.detect(); // Make sure that the BrickPi3 is communicating and that the firmware is compatible with the drivers.

  BP.set_sensor_type(PORT_1, SENSOR_TYPE_NXT_COLOR_BLUE);
  BP.set_sensor_type(PORT_3, SENSOR_TYPE_NXT_LIGHT_ON);

  //calibrate
  string regel;
  cout << "plaats sensor recht boven de lijn (zwart) en voer in a gevolgd door enter" << endl;
  cin >> regel;
  BP.get_sensor(PORT_1, mycolor);
  BP.get_sensor(PORT_3, mylight);
  MIN_LIGHT = mylight.reflected;
  MIN_RGB = mycolor.reflected_blue;
  cout << "MIN_RGB = " << MIN_RGB << endl;
  cout << "MIN_LIGHT = " << MIN_LIGHT << endl;
  cout << "plaats sensor helemaal naast de lijn (wit) en voer in b gevolgd door enter" << endl;
  cin >> regel;
  BP.get_sensor(PORT_1, mycolor);
  BP.get_sensor(PORT_3, mylight);
  MAX_RGB = mycolor.reflected_blue;
  MIN_LIGHT = mylight.reflected;
  cout << "MAX_RGB = " << MAX_RGB << endl;
  cout << "MAX_LIGHT = " << MAX_LIGHT << endl;
  cout << "plaats het voertuig met de sensor half boven de lijn en voer in c gevolgd door enter" << endl;
  cin >> regel;

  int16_t RGBval;
  int16_t lightval;
  int16_t power = 20;
  while(true){
    RGBval = getRGB();
    lightval = getlight();
    cout << lightval << RGBval << endl;
    sleep(1);
//    cout << lightval << endl;
//    if (RGBval <= 50){
//      BP.set_motor_power(PORT_B, (RGBval*power/50)-(RGBval*power/70));
//      BP.set_motor_power(PORT_C, (power+10)+(lightval/50));
//    }
//    if (lightval >= 50){
//      BP.set_motor_power(PORT_B, (power+10)+((100-RGBval)/50));
//      BP.set_motor_power(PORT_C, ((100-RGBval)*power/50)-(RGBval*power/70));
//   }
//    usleep(100000);
 } 
 }                        
 
 void exit_signal_handler(int signo){
  if(signo == SIGINT){
    BP.reset_all();    // Reset everything so there are no run-away motors
    BP.set_sensor_type(PORT_1, SENSOR_TYPE_NONE);  //Doesn't work, sorry.
    exit(-2);
  }
}
