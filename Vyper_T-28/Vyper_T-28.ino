//libraries
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SoftwareSerial.h>

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

//pin stuff
const byte SENSOR_PIN = 0,
          LASER_1_PIN = 8, LASER_2_PIN = 12,
          TOGGLE_MODE_INCREMENT_BUTTON_PIN = 2, TOGGLE_MODE_DECREMENT_BUTTON_PIN = 4, 
          VOLT_METER_PIN = 3,
          MAG_RELEASE_SWITCH_PIN = 7,
          DISPLAY_SCK_PIN = 5, DISPLAY_SDA_PIN = 4;

//photo resistor stuff
const int HIGH_VAL = 40, LOW_VAL = 1023;
byte lastValue = 0, readPhotoSensor, lastPhotoState;

//chrono stuff
boolean isTimerRunning = false;
double startTime, endTime, lastStartTime;

//button stuff
byte toggleModeIncrementButtonState = 0, toggleModeLastIncrementButtonState = 0,
        toggleModeDecrementButtonState = 0, toggleModeLastDecrementButtonState = 0,
        mode = 1; 

//ammo counter stuff
const byte MAX_AMMO = 18;
byte currentAmmo = 18;
boolean isDartThrough = false;

//change mag stuff
byte magReleaseState = 0, magReleaseLastState = 0;
boolean isMagIn = false;

//include all functions in helper file, but still have access to globals
#include "helper.h"

void setup() {
  Serial.begin(9600);

  //initialize display stuff
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    
  pinMode(TOGGLE_MODE_INCREMENT_BUTTON_PIN, INPUT);
  pinMode(TOGGLE_MODE_DECREMENT_BUTTON_PIN, INPUT);
  pinMode(MAG_RELEASE_SWITCH_PIN, INPUT);

  pinMode(LASER_1_PIN, OUTPUT);
  pinMode(LASER_2_PIN, OUTPUT);
  
}

void loop() {
  //toggle mode button stuff
  toggleModeIncrementButtonState = digitalRead(TOGGLE_MODE_INCREMENT_BUTTON_PIN);
  if (toggleModeLastIncrementButtonState != toggleModeIncrementButtonState && toggleModeIncrementButtonState == HIGH) {
    if (mode < 4) {
      mode++;
    } else {
      mode = 0;
    }
    checkMode();
  }
  toggleModeLastIncrementButtonState = toggleModeIncrementButtonState;

  toggleModeDecrementButtonState = digitalRead(TOGGLE_MODE_DECREMENT_BUTTON_PIN);
  if (toggleModeLastDecrementButtonState != toggleModeDecrementButtonState && toggleModeDecrementButtonState == HIGH) {
    if (mode > 1) {
      mode --;
    } else {
      mode = 4;
    }
    checkMode();
  }
  toggleModeLastDecrementButtonState = toggleModeDecrementButtonState;

  //photo resistor stuff
  readPhotoSensor = analogRead(SENSOR_PIN);
  if (mode % 4 == 0) {
    chrono();
  } else if (mode % 4 == 1) {
    ammoCounter();
  } else if (mode % 4 == 2) {
    rateOfFire();
  } else if (mode % 4 == 3) {
    voltMeter(analogRead(VOLT_METER_PIN));
  }
  lastPhotoState = readPhotoSensor;
  
  magReleaseState = digitalRead(MAG_RELEASE_SWITCH_PIN);
//  changeMag();
  magReleaseLastState = magReleaseState;
}


