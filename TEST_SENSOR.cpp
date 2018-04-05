#include "BrickPi3.h" // for BrickPi3
#include <iostream>      // for cin and cout
#include <unistd.h>     // for usleep
#include <signal.h>     // for catching exit signals
#include <string>       // for keeping track of the state

using namespace std;

BrickPi3 BP;

void exit_signal_handler(int signo);

void turn_left(void){
	BP.set_motor_power(PORT_B, 0);
	BP.set_motor_power(PORT_C, 0);
	BP.set_motor_position_relative(PORT_B, -600);
	BP.set_motor_position_relative(PORT_C, 600);
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
	forward(2);
	turn_left();
}

void dodge(sensor_ultrasonic_t Ultrasonic2){
	int done = 0;
	move_aside();
	while(done == 0){
		forward(3);
		turn_left();
		if(BP.get_sensor(PORT_2, Ultrasonic2) == 0){
			if(Ultrasonic2.cm < 30){
				turn_right();
			}
			else{
				forward(2);
				turn_right();
				done ++;
			}
 		}
	}
}

int main(){
  signal(SIGINT, exit_signal_handler);
 
  BP.detect();

  BP.set_sensor_type(PORT_2, SENSOR_TYPE_NXT_ULTRASONIC);

  sensor_ultrasonic_t Ultrasonic2;

  cout << "test" << endl;
  
  while(true){
	BP.set_motor_power(PORT_B, 30);
	BP.set_motor_power(PORT_C, 30);
        if(BP.get_sensor(PORT_2, Ultrasonic2) == 0){
		if(Ultrasonic2.cm < 30){
			cout << "minder" << endl;
			dodge(Ultrasonic2);
		}
 	}
  }

}

void exit_signal_handler(int signo){
  if(signo == SIGINT){
    BP.reset_all();    // Reset everything so there are no run-away motors
    BP.set_sensor_type(PORT_1, SENSOR_TYPE_NONE);  //Doesn't work, sorry.
    exit(-2);
  }
}
