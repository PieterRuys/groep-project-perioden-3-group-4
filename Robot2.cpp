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
  return 100-((100*(val - MinLight))/(MaxLight - MinLight));
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
	BP.set_motor_position_relative(PORT_B, 500);
	BP.set_motor_position_relative(PORT_C, -500);
	sleep(1);	
}

void forward(int time){
	BP.set_motor_power(PORT_B, 30);
	BP.set_motor_power(PORT_C, 30);
	sleep(time);
}
bool get_line(){
	if(getcolor() == 0 || getlight() == 100){
		return true;
	}
	return false;
}


void move_and_check(sensor_ultrasonic_t Ultrasonic2){
	int done = 0;
	while(done < 2){
		if(done == 0){
			turn_right();
			forward(2);
		}
		else{
			for(int x = 0; x < 30; x++){
				cout << "check" << endl;
				if(get_line()){
					done++;
				}
				usleep(100000);
			}
		}
		turn_left();
		if(BP.get_sensor(PORT_2, Ultrasonic2) == 0){
			if(Ultrasonic2.cm < 30){
				turn_right();
			}
			else{
				done++;
			}
 		}
	}
	BP.set_motor_power(PORT_B, 30);
	BP.set_motor_power(PORT_C, 30);
	int checkpoint = 0;
	while(checkpoint == 0){
		if(getcolor() == 0 || getlight() == 100){
			cout << "check" << endl;
			sleep(1);
			checkpoint++;
		}
	}
	BP.set_motor_power(PORT_B, 0);
	BP.set_motor_power(PORT_C, 0);
	while(checkpoint == 1){
		usleep(100000);
		if(getcolor() == 0){
			cout << "check2" << endl;
			checkpoint++;
		}
		else{
			BP.set_motor_position_relative(PORT_B, 20);
			BP.set_motor_position_relative(PORT_C, -20);
		}
	}
}

void dodge(sensor_ultrasonic_t Ultrasonic2){
	turn_right();
	move_and_check(Ultrasonic2);
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
  
  while(true){
    lightval = getlight();
    colorval = getcolor();
    cout << lightval << endl;
    cout << colorval << endl;
    if(BP.get_sensor(PORT_2, Ultrasonic2) == 0){
		    if(Ultrasonic2.cm < 15){
			    dodge(Ultrasonic2);
		    }
 	    }
    if (lightval <= 60){
      BP.set_motor_power(PORT_B, (30+(60-lightval)/2.0));
      BP.set_motor_power(PORT_C, (30-(70-lightval)/1.5));
    }
    else if (colorval <= 60){
      BP.set_motor_power(PORT_B, (30-(70-colorval)/1.5));
      BP.set_motor_power(PORT_C, (30+(60-colorval)/2.0));
    }
    else if(lightval > 60 && colorval > 60){
      BP.set_motor_power(PORT_B, 20);
      BP.set_motor_power(PORT_C, 20);
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
