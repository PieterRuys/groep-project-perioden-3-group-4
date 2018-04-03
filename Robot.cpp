#include "BrickPi3.h" // for BrickPi3
#include <iostream>      // for cin and cout
#include <unistd.h>     // for usleep
#include <signal.h>     // for catching exit signals

using namespace std;

BrickPi3 BP;

void exit_signal_handler(int signo);

uint16_t MIN;
uint16_t MAX;
sensor_color_t mycolor;

int main(){
  signal(SIGINT, exit_signal_handler); // register the exit function for Ctrl+C

  BP.detect(); // Make sure that the BrickPi3 is communicating and that the firmware is compatible with the drivers.

  BP.set_sensor_type(PORT_1, SENSOR_TYPE_NXT_COLOR_RED);

  //calibrate
  string regel;
  cout << "plaats sensor recht boven de lijn (zwart) en voer in a gevolgd door enter" << endl;
  cin >> regel;
  BP.get_sensor(PORT_1, mycolor);
  MIN = mycolor.reflected_red;
  cout << "MIN = " << MIN << endl;
  cout << "plaats sensor helemaal naast de lijn (wit) en voer in b gevolgd door enter" << endl;
  cin >> regel;
  BP.get_sensor(PORT_1, mycolor);
  MAX = mycolor.reflected_red;
  cout << "MAX = " << MAX << endl;
  cout << "plaats het voertuig met de sensor half boven de lijn en voer in c gevolgd door enter" << endl;
  cin >> regel;
