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
