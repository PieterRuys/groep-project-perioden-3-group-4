#include "BrickPi3.h" // for BrickPi3
#include <iostream>      // for cin and cout
#include <unistd.h>     // for usleep
#include <signal.h>     // for catching exit signals
#include <string>       // for keeping track of the state

using namespace std;

BrickPi3 BP;

void exit_signal_handler(int signo);

int main(){
  signal(SIGINT, exit_signal_handler);
 
  BP.detect();

  BP.set_sensor_type(PORT_2, SENSOR_TYPE_NXT_ULTRASONIC);

  sensor_ultrasonic_t Ultrasonic2;

  cout << "test" << endl;
  
  while(true){
        if(BP.get_sensor(PORT_2, Ultrasonic2) == 0){
		if(Ultrasonic2.cm < 30){
			cout << "minder" << endl;
		}
 	}
    sleep(1);
  }

}

void exit_signal_handler(int signo){
  if(signo == SIGINT){
    BP.reset_all();    // Reset everything so there are no run-away motors
    BP.set_sensor_type(PORT_1, SENSOR_TYPE_NONE);  //Doesn't work, sorry.
    exit(-2);
  }
}
