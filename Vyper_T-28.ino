//This program turns on led if laser shines on photo resistor
#include <SoftwareSerial.h>

//pin stuff
const int SENSOR_PIN = 0, LED_PIN = 13, BUTTON_PIN = 2, VOLT_METER_PIN = 1;

//photo resistor stuff
const int HIGH_VAL = 40, LOW_VAL = 1023;
int lastValue = 0, readPhotoSensor, lastPhotoState;

//chrono stuff
double startTime, endTime, lastStartTime;
boolean isTimerRunning = false;
const double FT_TO_CM = 0.23622;

//button stuff
int buttonState = 0, lastButtonState = 0, mode = 1;   //even = chrono, odd = ammo counter
//ammo counter stuff
int maxAmmo = 18, currentAmmo = 18;
boolean isDartThrough = false;

//volt meter stuff
const int R1_VAL = 100000, R2_VAL = 100000;


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
  }
  lastPhotoState = readPhotoSensor;

  //volt meter stuff
  if (mode % 3 == 3) {
    voltMeter();
  }

}

void chrono () {
  //Chrono Stuff
  if ((readPhotoSensor > HIGH_VAL) && !isTimerRunning) {   //if laser not shining
    isTimerRunning = true;
    //start timer
    startTime = micros();
  } else if ((readPhotoSensor < HIGH_VAL) && isTimerRunning)  {  //if laser shining
    isTimerRunning = false;
    endTime = micros();  
    
    //calculate chono readings
    double vel = FT_TO_CM/((endTime - startTime)/1000000);
    //print chrono readings
    Serial.println(vel);
  }
}

void ammoCounter () {
  //ammo counter stuff
  if ((readPhotoSensor > HIGH_VAL) && !isDartThrough) {   //if laser not shining
    isDartThrough = true;
  } else if ((readPhotoSensor < HIGH_VAL) && isDartThrough) {
    isDartThrough = false;
    if (currentAmmo > 0) {
      currentAmmo --;
      Serial.println(currentAmmo);
    }
  }
}

void voltMeter () {
  double value = analogRead(VOLT_METER_PIN);
  double voltageOut = (value * 3.3) / 1024.0;
  double voltageIn = voltageOut / (R2(R1 + R2));
  
  //display voltage
}

