#include "BrickPi3.h" // for BrickPi3
#include <iostream>      // for cin and cout
#include <unistd.h>     // for usleep
#include <signal.h>     // for catching exit signals

using namespace std;

BrickPi3 BP;

void exit_signal_handler(int signo);

uint16_t MIN;
uint16_t MAX;
sensor_light_t mylight;

int16_t getlight(){
  BP.get_sensor(PORT_3, mylight);
  int16_t val = mylight.reflected;
  return (100*(val - MIN))/(MAX - MIN);
}

int main(){
  signal(SIGINT, exit_signal_handler); // register the exit function for Ctrl+C

  BP.detect(); // Make sure that the BrickPi3 is communicating and that the firmware is compatible with the drivers.

  BP.set_sensor_type(PORT_3, SENSOR_TYPE_NXT_LIGHT_ON);
  
  string regel;
  cout << "plaats sensor recht boven de lijn (zwart) en voer in a gevolgd door enter" << endl;
  cin >> regel;
  BP.get_sensor(PORT_3, mylight);
  MIN = mylight.reflected;
  cout << "MIN = " << MIN << endl;
  cout << "plaats sensor helemaal naast de lijn (wit) en voer in b gevolgd door enter" << endl;
  cin >> regel;
  BP.get_sensor(PORT_3, mylight);
  MAX = mylight.reflected;
  cout << "MAX = " << MAX << endl;
  cout << "plaats het voertuig met de sensor half boven de lijn en voer in c gevolgd door enter" << endl;
  cin >> regel;
  
  int16_t lightval;
  int16_t power = 30;
  
  while(true){
    lightval = getlight();
    if (lightval <= 40){
      BP.set_motor_power(PORT_B, (lightval*power/50)-(lightval*power/60));
      BP.set_motor_power(PORT_C, (power)+(lightval/50));
    }
    if (lightval > 60){
      BP.set_motor_power(PORT_B, (power)+((100-lightval)/50));
      BP.set_motor_power(PORT_C, ((100-lightval)*power/50)-(lightval*power/60));
    }
    else{
      BP.set_motor_power(PORT_B, 30);
      BP.set_motor_power(PORT_C, 30);
    }
    usleep(100000);
 }   
  
}
  
  
 void exit_signal_handler(int signo){
  if(signo == SIGINT){
    BP.reset_all();    // Reset everything so there are no run-away motors
    BP.set_sensor_type(PORT_1, SENSOR_TYPE_NONE);  //Doesn't work, sorry.
    exit(-2);
  }
}
