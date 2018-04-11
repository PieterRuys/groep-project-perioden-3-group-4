#include "BrickPi3.h" // for BrickPi3
#include <iostream>      // for cin and cout
#include <unistd.h>     // for usleep
#include <signal.h>     // for catching exit signals

using namespace std;

BrickPi3 BP;

void exit_signal_handler(int signo);

uint16_t MIN1;
uint16_t MAX1;
uint16_t MIN2;
uint16_t MAX2;
sensor_color_t kleur1;
sensor_light_t kleur2;


//measure reflected light
//if the value is exactly in the middle between MIN and MAX the motors should have equal speed.
//if the value is closer to MIN motor on PORT_B should be a bit slower.
//if the value is closer to MAX motor on PORT_C should be a bit slower.
//return value is between 0 and 100
//0 means value MIN is measured so the sensor is above the line
//100 means value MAX is measured so the sensor is next to the line.
;
int16_t measureLight1() {

	BP.get_sensor(PORT_1, kleur1);
	uint16_t val = kleur1.reflected_red;
	if (val < MIN1) val = MIN1;
	if (val > MAX1) val = MAX1;
	return (100*(val - MIN1))/(MAX1 - MIN1);
}
int16_t measureLight2(){

	BP.get_sensor(PORT_3, kleur2);
	uint16_t val = kleur2.reflected;
	if (val < MIN2) val = MIN2;
	if (val > MAX2) val = MAX2;
	return (100*(val - MIN2))/(MAX2 - MIN2);
}


float I = 0.0;
float Pg = 3;
float Ig = 0.3;

//PI work function
int workPI(const int target,int input,float & I, float Pg, float Ig){
                int error = target - input;
                I += error;
                return Pg*error + Ig*I;
}

//Domain controller
int domain(int lower, int upper, int x){
	if (x > upper) x = upper;
	if (x < lower) x = lower;
	return x;
}




int main(){
  signal(SIGINT, exit_signal_handler); // register the exit function for Ctrl+C

  BP.detect(); // Make sure that the BrickPi3 is communicating and that the firmware is compatible with the drivers.

  BP.set_sensor_type(PORT_1, SENSOR_TYPE_NXT_COLOR_RED);
  BP.set_sensor_type(PORT_2, SENSOR_TYPE_NXT_ULTRASONIC);
  BP.set_sensor_type(PORT_3, SENSOR_TYPE_NXT_LIGHT_ON);

  sensor_ultrasonic_t Ultrasonic2;

  //calibrate
  string regel;
  cout << "SENSOR boven lijn (zwart) voer in 'a' enter" << endl;
  cin >> regel;
  BP.get_sensor(PORT_1, kleur1);
  MIN1 = kleur1.reflected_red;
  cout << "MIN1 = " << MIN1 << endl;
  BP.get_sensor(PORT_3, kleur2);
  MIN2 = kleur2.reflected;
  cout << "MIN2 = " << MIN2 << endl;
  cout << "SENSOR naast lijn (wit) voer in 'b'gevolgd door enter" << endl;
  cin >> regel;
  BP.get_sensor(PORT_1, kleur1);
  MAX1 = kleur1.reflected_red;
  cout << "MAX1 = " << MAX1 << endl;
  BP.get_sensor(PORT_3, kleur2);
  MAX2 = kleur2.reflected;
  cout << "MAX2 = " << MAX2 << endl;
  cout << "plaats het voertuig met de sensor half boven de lijn en voer in c gevolgd door enter" << endl;
  cin >> regel;
	int target = 65;



  //follow line
  int16_t lightval1;
  int16_t lightval2;
  int speed = 160;
  int steer = 0;
  while(true){
   cout << "target: " << target << "\n";

    lightval1 = measureLight1();
    lightval2 = measureLight2();

    if (lightval1 <= 20){
                BP.set_motor_dps(PORT_B, 150);
                BP.set_motor_dps(PORT_C, 0);
   }

    steer = workPI(target,lightval2,I,Pg,Ig);
    cout << lightval1 << "\n";
    cout << lightval2 << "\n";

    int motorpwr1 = domain(0,360,speed - steer);
    int motorpwr2 = domain(0,360,speed + steer);

    BP.set_motor_dps(PORT_B, motorpwr1);
    BP.set_motor_dps(PORT_C, motorpwr2);

    if(lightval2 >= 87){
	I = 0;
	BP.set_motor_dps(PORT_B, 150);
        BP.set_motor_dps(PORT_C, 0);
}


    if(BP.get_sensor(PORT_2, Ultrasonic2) == 0){
                cout << "Ultrasonic sensor (S2): "   << Ultrasonic2.cm << "cm" << endl;
    }
    if(Ultrasonic2.cm <= 20){
        BP.set_motor_dps(PORT_B, 0);
        BP.set_motor_dps(PORT_C, 0);
	sleep(1);
	BP.set_motor_dps(PORT_B, -180);
        BP.set_motor_dps(PORT_C, 180);
        sleep(1);
	BP.set_motor_dps(PORT_B, 120);
        BP.set_motor_dps(PORT_C, 75);

        while(true){
                 lightval1 = measureLight1();
                 lightval2 = measureLight2();

                if(lightval1 <= 55 || lightval2 <= 55){
                        break;
                }
        }


    }

usleep(80000);//sleep 80 ms
  }
}


// Signal handler that will be called when Ctrl+C is pressed to stop the program
void exit_signal_handler(int signo){
  if(signo == SIGINT){
    BP.reset_all();    // Reset everything so there are no run-away motors
    BP.set_sensor_type(PORT_1, SENSOR_TYPE_NONE);  //Doesn't work, sorry.
    exit(-2);
  }
}
