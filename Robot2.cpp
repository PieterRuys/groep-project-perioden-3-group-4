#include "BrickPi3.h" // for BrickPi3
#include <iostream>      // for cin and cout
#include <unistd.h>     // for usleep
#include <signal.h>     // for catching exit signals

using namespace std;

BrickPi3 BP;

void exit_signal_handler(int signo);

uint16_t MinLight;
uint16_t MinColor;
uint16_t MaxLight;
uint16_t MaxColor;
sensor_light_t mylight;
sensor_color_t mycolor;

int16_t getlight(){
  BP.get_sensor(PORT_3, mylight);
  int16_t val = mylight.reflected;
  if (val < MinLight) val = MinLight;
  if (val > MaxLight) val = MaxLight;
  return (100*(val - MinLight))/(MaxLight - MinLight);
}

int16_t getcolor(){
  BP.get_sensor(PORT_1, mycolor);
  uint16_t val = mycolor.reflected_red;
  if (val < MinColor) val = MinColor;
  if (val > MaxColor) val = MaxColor;
  return (100*(val - MinColor))/(MaxColor - MinColor);
}

void turn_left(void){
	BP.set_motor_power(PORT_B, 0);
	BP.set_motor_power(PORT_C, 0);
	BP.set_motor_position_relative(PORT_B, -500);
	BP.set_motor_position_relative(PORT_C, 500);
	sleep(1);
}

void turn_right(void){
	BP.set_motor_power(PORT_B, 0);
	BP.set_motor_power(PORT_C, 0);
	BP.set_motor_position_relative(PORT_B, 600);
	BP.set_motor_position_relative(PORT_C, -600);
	sleep(1);	
}

void forward(int time){
	BP.set_motor_power(PORT_B, 50);
	BP.set_motor_power(PORT_C, 50);
	sleep(time);
}

void move_aside(void){
	turn_right();
	forward(1.5);
	turn_left();
}

void dodge(sensor_ultrasonic_t Ultrasonic2){
	int done = 0;
	move_aside();
	while(done == 0){
		forward(1);
		turn_left();
		if(BP.get_sensor(PORT_2, Ultrasonic2) == 0){
			if(Ultrasonic2.cm < 20){
				turn_right();
			}
			else{
				while(true){
					//hier forward tot de sensoren een lijn zien dan turn_right(); en  dan verder gaan met het nromale protocol
					BP.set_motor_power(PORT_B, 20);
					BP.set_motor_power(PORT_C, 20);
					if(getcolor() < 40){
						cout << "check" << endl;
						BP.set_motor_power(PORT_B, 30);
						BP.set_motor_power(PORT_C, 30);
						sleep(0.5);
						turn_right();
						break;
					}
				}
				done++;
			}
 		}
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
  
  int16_t lightval;
  int16_t colorval;
  int16_t power = 20;
  
  while(true){
    lightval = getlight();
    colorval = getcolor();
    cout << lightval << endl;
    cout << colorval << endl;
    if(BP.get_sensor(PORT_2, Ultrasonic2) == 0){
		    if(Ultrasonic2.cm < 20){
			    dodge(Ultrasonic2);
		    }
 	    }
    if (lightval > 30){
      BP.set_motor_power(PORT_B, power+((lightval-30)/1.5));
      BP.set_motor_power(PORT_C, power-((lightval-30)/1.5));
    }
    if (colorval < 70){
      BP.set_motor_power(PORT_B, power-((70-lightval)/1.5));
      BP.set_motor_power(PORT_C, power+((70- lightval)/1.5));
    }
    if(lightval < 30 && colorval > 50){
      BP.set_motor_power(PORT_B, power);
      BP.set_motor_power(PORT_C, power);
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
