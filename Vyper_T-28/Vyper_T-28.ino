//This program turns on led if laser shines on photo resistor
#include <SoftwareSerial.h>

//pin stuff
const int SENSOR_PIN = 0, LED_PIN = 13, BUTTON_PIN = 2, VOLT_METER_PIN = 1;

//photo resistor stuff
const int HIGH_VAL = 40, LOW_VAL = 1023;
int lastValue = 0, readPhotoSensor, lastPhotoState;

//chrono stuff
boolean isTimerRunning = false;
double startTime, endTime, lastStartTime;
const double FT_TO_CM = 0.23622;

//button stuff
int buttonState = 0, lastButtonState = 0, mode = 1;   //even = chrono, odd = ammo counter
//ammo counter stuff
const int MAX_AMMO = 18;
int currentAmmo = 18;
boolean isDartThrough = false;

void setup() {
  Serial.begin(9600);
  
  pinMode(LED_PIN, OUTPUT);   
  
  pinMode(BUTTON_PIN, INPUT);
  pinMode(VOLT_METER_PIN, INPUT);
}

void loop() {
  //button stuff
  buttonState = digitalRead(BUTTON_PIN);
  if (buttonState != lastButtonState && buttonState == HIGH) {
    mode++;
    currentAmmo = 18;
  }
  lastButtonState = buttonState;

  //photo resistor stuff
  readPhotoSensor = analogRead(SENSOR_PIN);
  if (mode % 3 == 0) {
    chrono();
  } else if (mode % 3 == 1) {
    ammoCounter();
  } else if (mode % 3 == 2) {
    rateOfFire();
  }
  lastPhotoState = readPhotoSensor;

  //volt meter stuff
  if (mode % 3 == 3) {
    voltMeter(analogRead(VOLT_METER_PIN));
  }

}

void chrono () {
  //Chrono Stuff
  if ((readPhotoSensor > HIGH_VAL) && !isTimerRunning) {   //if laser not shining
    isTimerRunning = true;
    startTime = micros();    //"start timer"
  } else if ((readPhotoSensor < HIGH_VAL) && isTimerRunning)  {  //if laser shining
    isTimerRunning = false;
    endTime = micros();    //"end timer"
    double vel = FT_TO_CM/((endTime - startTime)/1000000);    //convert 7.2cm/microsecont to fps
    //print chrono readings
    Serial.println(vel);
  }
}

int ammoCounter () {
  //ammo counter stuff
  if ((readPhotoSensor > HIGH_VAL) && !isDartThrough) {   //if laser not shining
    isDartThrough = true;
  } else if ((readPhotoSensor < HIGH_VAL) && isDartThrough) {
    isDartThrough = false;
    if (currentAmmo > 0) {
      currentAmmo --;
      return currentAmmo;
      Serial.println(currentAmmo);
    } else if (currentAmmo == 0) {
      return currentAmmo;
    }
  }
}

void rateOfFire () {
//  if (!ammoCounter()) {
//    rateOfFire();
//  } else {
//    
//  }
}

void voltMeter (double value) {
  const int R1_VAL = 100000, R2_VAL = 100000;

  double voltageOut = (value * 3.3) / 1024.0;
  double voltageIn = voltageOut / (R2_VAL * (R1_VAL + R2_VAL));
  
  //display voltage
}

