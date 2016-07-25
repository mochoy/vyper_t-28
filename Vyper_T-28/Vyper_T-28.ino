//This program turns on led if laser shines on photo resistor
#include <SoftwareSerial.h>

//pin stuff
const int SENSOR_PIN = 0, TOGGLE_MODE_BUTTON_PIN = 2, VOLT_METER_PIN = 7, MAG_RELEASE_SWITCH_PIN = 4, DISPLAY_SCK_PIN = 5, DISPLAY_SDA_PIN = 4;

//photo resistor stuff
const int HIGH_VAL = 40, LOW_VAL = 1023;
int lastValue = 0, readPhotoSensor, lastPhotoState;

//chrono stuff
boolean isTimerRunning = false;
double startTime, endTime, lastStartTime;
const double FT_TO_CM = 0.23622;

//button stuff
int toggleModeButtonState = 0, toggleModeLastButtonState = 0, mode = 1;   //even = chrono, odd = ammo counter

//ammo counter stuff
const int MAX_AMMO = 18;
int currentAmmo = 18;
boolean isDartThrough = false;

//change mag stuff
int magReleaseState = 0, magReleaseLastState = 0;
boolean isMagIn = false;

void setup() {
  Serial.begin(9600);
    
  pinMode(TOGGLE_MODE_BUTTON_PIN, INPUT);
  pinMode(MAG_RELEASE_SWITCH_PIN, INPUT);
  pinMode(VOLT_METER_PIN, INPUT);
}

void loop() {
  //button stuff
  toggleModeButtonState = digitalRead(TOGGLE_MODE_BUTTON_PIN);
  if (toggleModeButtonState != toggleModeLastButtonState && toggleModeButtonState == HIGH) {
    mode++;
    if (mode % 4 == 0) {
      Serial.println("chrono");
    } else if (mode % 4 == 1) {
      Serial.println("ammo counter");
    } else if (mode % 4 == 2) {
      Serial.println("rate of fire");
    } else if (mode % 4 == 3) {
        Serial.println("volt meter");
    }
  }
  toggleModeLastButtonState = toggleModeButtonState;

  //photo resistor stuff
  readPhotoSensor = analogRead(SENSOR_PIN);
  if (mode % 4 == 0) {
    chrono();
  } else if (mode % 4 == 1) {
    ammoCounter();
  } else if (mode % 4 == 2) {
    rateOfFire();
  }
  lastPhotoState = readPhotoSensor;

  //volt meter stuff
  if (mode % 4 == 3) {
    int value = analogRead(VOLT_METER_PIN);
    voltMeter(value);
  }
  
  magReleaseState = digitalRead(MAG_RELEASE_SWITCH_PIN);
  changeMag();
  magReleaseLastState = magReleaseState;
}

void chrono () {
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

void ammoCounter () {  
  if ((readPhotoSensor > HIGH_VAL) && !isDartThrough) {   //if laser not shining
    isDartThrough = true;
  } else if ((readPhotoSensor < HIGH_VAL) && isDartThrough) {
    isDartThrough = false;
    if (currentAmmo > 0) {
      currentAmmo --;
    } 
  }
}

void rateOfFire () {
  if (currentAmmo >= 0) {   //make sure still in rate of fire mode and there are still darts
    if ((readPhotoSensor > HIGH_VAL) && !isDartThrough) {   //if dart passes
      isDartThrough = true;
    } else if ((readPhotoSensor < HIGH_VAL) && isDartThrough) {   //if dart passes
      isDartThrough = false;
      
      if (currentAmmo == MAX_AMMO) {   //"start timer" when 18 darts
        startTime = micros();  
      } else if (currentAmmo > 0) {
      } else if (currentAmmo == 0) {
        endTime = micros();
        double rof = 18 / ((endTime - startTime)/1000000);   //calculate rate of fire
        Serial.println(rof);
      }
      currentAmmo--;
    }
  }   //if validation
}   //function

  const int R1_VAL = 100000, R2_VAL = 100000;


void voltMeter (int value) {

  double voltageOut = (value * 5.0) / 1024.0;
  double voltageIn = voltageOut / (R2_VAL / (R1_VAL + R2_VAL));
  
  //display voltage
//  if (voltageIn > 0) {
    Serial.println(voltageIn);
//  }
}

void changeMag () {
  if ((magReleaseState != magReleaseLastState) && magReleaseState == HIGH) {   //when switched pressed
    if (!isMagIn) {   //if mag wasn't in last time this was checked
      currentAmmo = MAX_AMMO;
      //display ammo
      Serial.println(currentAmmo);
    }
    isMagIn = true;
  } else if ((magReleaseState != magReleaseLastState) && magReleaseState == LOW) {    //when switch isn't pressed
    isMagIn = false;
  }
 }

