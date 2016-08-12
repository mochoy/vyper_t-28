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
const byte HIGH_VAL_1 = 80;
byte lastValue = 0, readPhotoSensor, lastPhotoState;

//chrono stuff
boolean isTimerRunning = false;
double startTime, endTime, lastStartTime;

//button stuff
byte toggleModeIncrementButtonState = 0, toggleModeLastIncrementButtonState = 0,
        toggleModeDecrementButtonState = 0, toggleModeLastDecrementButtonState = 0,
        mode = 0, lastMode = 0; 

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
  lastMode = mode;
  
  //toggle mode button stuff
  toggleModeIncrementButtonState = digitalRead(TOGGLE_MODE_INCREMENT_BUTTON_PIN);
  if (toggleModeLastIncrementButtonState != toggleModeIncrementButtonState && toggleModeIncrementButtonState == HIGH) {
    mode ++;
    if (mode > 3) {
      mode = 0;
    }
  }
  toggleModeLastIncrementButtonState = toggleModeIncrementButtonState;

  toggleModeDecrementButtonState = digitalRead(TOGGLE_MODE_DECREMENT_BUTTON_PIN);
  if (toggleModeLastDecrementButtonState != toggleModeDecrementButtonState && toggleModeDecrementButtonState == HIGH) {
    mode --;
    if (mode > 200) {   //if mode less than 0, value overflows to 255
      mode = 3;
    }
  }  
  toggleModeLastDecrementButtonState = toggleModeDecrementButtonState;

  //photo resistor stuff
  readPhotoSensor = analogRead(SENSOR_PIN);

  doMode();

  
  lastPhotoState = readPhotoSensor;
  
  magReleaseState = digitalRead(MAG_RELEASE_SWITCH_PIN);
//  changeMag();
  magReleaseLastState = magReleaseState;
}


