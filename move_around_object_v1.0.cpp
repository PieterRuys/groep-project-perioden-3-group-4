#include "BrickPi3.h" // for BrickPi3
#include <iostream>      // for cin and cout
#include <unistd.h>     // for usleep
#include <signal.h>     // for catching exit signals
#include <string>       // for keeping track of the state

using namespace std;

BrickPi3 BP;

void stop(void)
{
	 BP.set_motor_power(PORT_B, 0);
	 BP.set_motor_power(PORT_C, 0);
}

void fwd(void)
{
	BP.set_motor_power(PORT_B, 20);
	BP.set_motor_power(PORT_C, 20);
	usleep(500000);
	BP.set_motor_power(PORT_B, 40);
	BP.set_motor_power(PORT_C, 40);
	usleep(500000);
	BP.set_motor_power(PORT_B, 50);
	BP.set_motor_power(PORT_C, 50);
	sleep(2);
	stop();
}

void rgt(void){
    BP.set_motor_position_relative(PORT_B, 270);
    BP.set_motor_position_relative(PORT_C, -270);
}

void lft(void){
    BP.set_motor_position_relative(PORT_B, -270);
    BP.set_motor_position_relative(PORT_C, 270);
}


string check_for_object(){
    if(Ultrasonic2.cm > 10){
       fwd(); 
       return "IDLE";
    }
    return "OBJECT";
}

string move_asside(){
    fwd();
    lft();
    return "check_again";
}

string move_around(){
    fwd();
    rgt();
    return "check_distance";
}

int main()
{

BP.set_sensor_type(PORT_2, SENSOR_TYPE_NXT_ULTRASONIC);

sensor_ultrasonic_t Ultrasonic2;

string state = "IDLE";
    while(true){
        if(state == "IDLE"){
            state = check_for_object();
        }
        else if(state == "OBJECT"){
	    rgt();
            state = move_asside();
        }
        else if(state == "check_again"){
            if(Ultrasonic2.cm > 10){
                state = "OBJECT";
            }
            else{
                state = "AROUND_OBJECT";
            }
        }
        else if(state == "AROUND_OBJECT"){
            state = move_around();
        }
        else if(state == "check_distance"){
            if(Ultrasonic2.cm > 10){
                fwd(); 
                rgt();
                state = "IDLE";
            }
            else{
                state = "AROUND_OBJECT";
            }
    }
    }
}
