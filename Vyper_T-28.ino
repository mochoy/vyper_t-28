//This program turns on led if laser shines on photo resistor
#include <SoftwareSerial.h>

const int SENSOR_PIN = 0, LED_PIN = 13, BUTTON_PIN = 2;

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

void setup() {
  Serial.begin(9600);
  
  pinMode(LED_PIN, OUTPUT);   
  pinMode(BUTTON_PIN, INPUT);
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
  if (mode%2 == 0) {
    chrono();
  } else if (mode % 2 == 1) {
    ammoCounter();
  }
  lastPhotoState = readPhotoSensor;

  

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
  if ((readPhotoSensor > HIGH_VAL) && !isTimerRunning) {   //if laser not shining
    if (currentAmmo > 0) {
      currentAmmo --;
    }
    Serial.println(currentAmmo);
  } 
}

