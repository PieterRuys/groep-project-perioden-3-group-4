#include "BrickPi3.h"	// for BrickPi3
#include <iostream>	// for cin and cout
#include <unistd.h>	// for usleep
#include <signal.h>	// for catching exit signals

using namespace std;

BrickPi3 BP;

void exit_signal_handler(int signo);

uint16_t MinLight;
uint16_t MinColor;
uint16_t MaxLight;
uint16_t MaxColor;
sensor_ultrasonic_t Ultrasonic2;
sensor_light_t mylight;
sensor_color_t mycolor;
int16_t lightval;
int16_t colorval;


int16_t getlight(){	// Calculates the percentage of light based on the value received from the first sensor and the min and max values
	BP.get_sensor(PORT_3, mylight);
	int16_t val = mylight.reflected;
	if (val < MinLight) val = MinLight;
	if (val > MaxLight) val = MaxLight;
	return 100-((100*(val - MinLight))/(MaxLight - MinLight));
}


int16_t getcolor(){	// Calculates the percentage of light based on the value received from the second sensor and the min and max values
	BP.get_sensor(PORT_1, mycolor);
	uint16_t val = mycolor.reflected_red;
	if (val < MinColor) val = MinColor;
	if (val > MaxColor) val = MaxColor;
	return (100*(val - MinColor))/(MaxColor - MinColor);
}

void get_values(){	// Calculates the reflected values for the colors black and white
	// These values get stored in the max and min variables both sensors have one of each
	// Waits for input between each scan for ease of use
	string regel;
	cout << "plaats sensor recht boven de lijn (zwart) en voer in a gevolgd door enter" << endl;
	cin >> regel;
	BP.get_sensor(PORT_3, mylight);
	BP.get_sensor(PORT_1, mycolor);
	MaxLight = mylight.reflected;
	MinColor = mycolor.reflected_red;
	cout << "MaxLight =" << MaxLight << endl;
	cout << "MinColor =" << MinColor << endl;
	cout << "Plaats de sensoren recht boven de lijn (wit) en voer in a gevolgd door enter" << endl;
	cin >> regel;
	BP.get_sensor(PORT_3, mylight);
	BP.get_sensor(PORT_1, mycolor);
	MinLight = mylight.reflected;
	MaxColor = mycolor.reflected_red;
	cout << "MinLight =" << MinLight << endl;
	cout << "MaxColor =" << MaxColor << endl;
	cout << "Plaats de robot op de lijn en voer in a gevolgd door enter" << endl;
	cin >> regel;	
}

void turn_left(void){	// This function first stops the robot, the sets te postion in such a way that the robt makes a 90 degrees turn left
	BP.set_motor_power(PORT_B, 0);
	BP.set_motor_power(PORT_C, 0);
	BP.set_motor_position_relative(PORT_B, -500);
	BP.set_motor_position_relative(PORT_C, 500);
	sleep(1);
}

void turn_right(void){	// This function first stops the robot, the sets te postion in such a way that the robt makes a 90 degrees turn right
	BP.set_motor_power(PORT_B, 0);
	BP.set_motor_power(PORT_C, 0);
	BP.set_motor_position_relative(PORT_B, 550);
	BP.set_motor_position_relative(PORT_C, -550);
	sleep(1);	
}

void forward(int time){	// This funtion sets the power to drive forward
	BP.set_motor_power(PORT_B, 30);
	BP.set_motor_power(PORT_C, 30);
	sleep(time);
}
bool get_line(){	// Returns true if one of the sensors is on black
	if(getcolor() == 0 || getlight() == 0){
		return true;
	}
	return false;
}

void turn_search_line_l(){	// This code slowly turns left while looking for the line
int checkpoint = 1;
int counter = 0;
   	BP.set_motor_power(PORT_B, 0);
	BP.set_motor_power(PORT_C, 0);
	for(int x = 0; x < 27; x++){
		if(getlight() <= 0){
			break
		}
        	usleep(100000);
		if(getlight() >= 0){
			BP.set_motor_position_relative(PORT_B, -20);
			BP.set_motor_position_relative(PORT_C, 20);
			counter ++;
		}
	}
}

void turn_back(int &done, sensor_ultrasonic_t Ultrasonic2){	// Looks if there is an obstacle in the way, if there is it will turn right and drive forward otherwise it wil just drive forward
    turn_search_line_l();
    if(BP.get_sensor(PORT_2, Ultrasonic2) == 0){
        if(Ultrasonic2.cm < 30){
            turn_right();
        }
        else{
            done++;
        }
    }
}

void check_for_line(int &done){	// While the robot drives forward for 3 seconds it checks if it has passed the line
    forward(0);
    for(int x = 0; x < 300; x++){	// Because the sleep time is 10000 microseconds and this loop gets repeated 30 times the overal time is 3 seconds
        if(get_line()){
            done = 3;
            break;
        }
        usleep(10000);
    }
}

void drive_until_line(int &checkpoint){	// Here the code drives forward until it finds the line
    forward(0);
	while(checkpoint == 0){
		if(getcolor() == 0 || getlight() == 0){
			usleep(1000000);	// If the line is found the robot drives forward until it's body is on the line
			checkpoint++;
		}
	}
}

void turn_search_line(int &checkpoint){	// This code slowly turns right while looking for the line
    BP.set_motor_power(PORT_B, 0);
	BP.set_motor_power(PORT_C, 0);
	while(checkpoint == 1){
		if(getcolor() <= 10){
			checkpoint++;
		}
        usleep(100000);
		if(getcolor() > 10){
			BP.set_motor_position_relative(PORT_B, 20);
			BP.set_motor_position_relative(PORT_C, -20);
		}
	}
}


void move_and_check(sensor_ultrasonic_t Ultrasonic2){	// This is the code wich calls to most other codes and looks for different states
    int done = 0;
    int checkpoint = 0;
    turn_right();
    while(done < 2){
		if(done == 0){
			forward(2);
		}
		else{
			check_for_line(done);
		}
		if(done != 3){
			turn_back(done, Ultrasonic2);
		}
	}
    drive_until_line(checkpoint);
    turn_search_line(checkpoint);
}

void control_motors(){	// Checks all the sensors and gives the motors power based on the values from the sensors.
	lightval = getlight();	// Calculates the current percentage of light on the first sensor
	colorval = getcolor();	// Calculates the current percentage of light on the second sensor
	if(BP.get_sensor(PORT_2, Ultrasonic2) == 0){
			if(Ultrasonic2.cm < 15){
			    move_and_check(Ultrasonic2);
			}
	}
	// Turning right
	if (lightval <= 60){	// If the percentage of light from the first sensor is below or equal to 60 the motors get power based on the percentage of light
		BP.set_motor_power(PORT_B, (30+(60-lightval)/2.0)); // With the left motor getting more power than the right motor
		BP.set_motor_power(PORT_C, (30-(70-lightval)/1.5));
	}
	// Turning left
	else if (colorval <= 60){	// If the percentage of light from the second sensor is below or equal to 60 the motors get power based on the percentage of light
		BP.set_motor_power(PORT_B, (30-(70-colorval)/1.5)); // With the right motor getting more power than the left motor
		BP.set_motor_power(PORT_C, (30+(60-colorval)/2.0));
	}
	// Going straight
	else if(lightval > 60 && colorval > 60){	// If the percentage of light from both of the sensors are above 60 both of the motors get an equal amount of power
		BP.set_motor_power(PORT_B, 20);
		BP.set_motor_power(PORT_C, 20);
	}
	usleep(100000);
}  


int main(){
	signal(SIGINT, exit_signal_handler);	// register the exit function for Ctrl+C

	BP.detect();	// Make sure that the BrickPi3 is communicating and that the firmware is compatible with the drivers.
  
	// Sets the ports of the BrickPi3 for the use of the different sensors
	// The first value represents the port that is being used and the second value represents the type of sensor that is being used
	BP.set_sensor_type(PORT_3, SENSOR_TYPE_NXT_LIGHT_ON);
	BP.set_sensor_type(PORT_2, SENSOR_TYPE_NXT_ULTRASONIC);
	BP.set_sensor_type(PORT_1, SENSOR_TYPE_NXT_COLOR_RED);
 
	get_values();
	while(true){
		control_motors();
	}
}
  
  
void exit_signal_handler(int signo){
	if(signo == SIGINT){
		BP.reset_all();	// Reset everything so there are no run-away motors
		BP.set_sensor_type(PORT_1, SENSOR_TYPE_NONE);	//Doesn't work, sorry.
		exit(-2);
	}
}
