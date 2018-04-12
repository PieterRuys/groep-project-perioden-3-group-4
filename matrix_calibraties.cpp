#include "BrickPi3.h" // for BrickPi3
#include <iostream>      // for cin and cout
#include <unistd.h>     // for usleep
#include <signal.h>     // for catching exit signals

using namespace std;

void exit_signal_handler(int signo);

uint16_t MinLight;
uint16_t MinColor;
uint16_t MaxLight;
uint16_t MaxColor;
sensor_light_t mylight;
sensor_color_t mycolor;

BrickPi3 BP;

void turn_left(void){
	BP.set_motor_power(PORT_B, 0);
	BP.set_motor_power(PORT_C, 0);
	BP.set_motor_position_relative(PORT_B, -540);
	BP.set_motor_position_relative(PORT_C, 540);
	sleep(1);
}

void turn_right(void){
	BP.set_motor_power(PORT_B, 0);
	BP.set_motor_power(PORT_C, 0);
	BP.set_motor_position_relative(PORT_B, 540);
	BP.set_motor_position_relative(PORT_C, -540);
	sleep(1);	
}

void forward(int time){
	BP.set_motor_power(PORT_B, 30);
	BP.set_motor_power(PORT_C, 30);
	sleep(time);
}

int16_t getlight(){
  BP.get_sensor(PORT_3, mylight);
  int16_t val = mylight.reflected;
  if (val < MinLight) val = MinLight;
  if (val > MaxLight) val = MaxLight;
  return 100-((100*(val - MinLight))/(MaxLight - MinLight));
}

int16_t getcolor(){
  BP.get_sensor(PORT_1, mycolor);
  uint16_t val = mycolor.reflected_red;
  if (val < MinColor) val = MinColor;
  if (val > MaxColor) val = MaxColor;
  return (100*(val - MinColor))/(MaxColor - MinColor);
}

bool next_crosing_free(sensor_ultrasonic_t Ultrasonic2){
 	if(BP.get_sensor(PORT_2, Ultrasonic2) == 0){
		if(Ultrasonic2.cm < 10){
			return true;
		}
		else{
			return false;
		}
  	}
}

void forward_one_step(){
  while(true){

    int16_t lightval = getlight();
cout << lightval << endl;
    
    if(lightval <= 5){
      //drive until you are on the crossing
      sleep(1);
      BP.set_motor_power(PORT_B, 0);
      BP.set_motor_power(PORT_C, 0);
      break;
    }
   
    else{
      BP.set_motor_power(PORT_B, 30);
      BP.set_motor_power(PORT_C, 30);
    }
    usleep(100000);
  }
}

int main(){
  signal(SIGINT, exit_signal_handler); // register the exit function for Ctrl+C

  BP.detect(); // Make sure that the BrickPi3 is communicating and that the firmware is compatible with the drivers.

  BP.set_sensor_type(PORT_3, SENSOR_TYPE_NXT_LIGHT_ON);
  BP.set_sensor_type(PORT_2, SENSOR_TYPE_NXT_ULTRASONIC);
  BP.set_sensor_type(PORT_1, SENSOR_TYPE_NXT_COLOR_RED);
  
  sensor_ultrasonic_t Ultrasonic2;
  
  string regel;
  cout << "plaats sensor recht boven de lijn (zwart) en voer in a gevolgd door enter" << endl;
  cin >> regel;
  BP.get_sensor(PORT_3, mylight);
  BP.get_sensor(PORT_1, mycolor);
  MaxLight = mylight.reflected;
  MinColor = mycolor.reflected_red;
  cout << "MaxLight =" << MaxLight << endl;
  cout << "MinColor =" << MinColor << endl;
  cout << "plaats sensor recht boven de lijn (wit) en voer in a gevolgd door enter" << endl;
  cin >> regel;
  BP.get_sensor(PORT_3, mylight);
  BP.get_sensor(PORT_1, mycolor);
  MinLight = mylight.reflected;
  MaxColor = mycolor.reflected_red;
  cout << "MinLight =" << MinLight << endl;
  cout << "MaxColor =" << MaxColor << endl;
while(true){
forward_one_step();
turn_right();
turn_right();
}
  
  
  

  
  



}
  
  
 void exit_signal_handler(int signo){
  if(signo == SIGINT){
    BP.reset_all();    // Reset everything so there are no run-away motors
    BP.set_sensor_type(PORT_1, SENSOR_TYPE_NONE);  //Doesn't work, sorry.
    exit(-2);
  }
}
