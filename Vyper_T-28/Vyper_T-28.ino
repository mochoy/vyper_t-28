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
const byte SENSOR_PIN = 0, TOGGLE_MODE_INCREMENT_BUTTON_PIN = 2, TOGGLE_MODE_DECREMENT_BUTTON_PIN = 4, 
            VOLT_METER_PIN = 3, MAG_RELEASE_SWITCH_PIN = 7, DISPLAY_SCK_PIN = 5, DISPLAY_SDA_PIN = 4;

//photo resistor stuff
const int HIGH_VAL = 40, LOW_VAL = 1023;
byte lastValue = 0, readPhotoSensor, lastPhotoState;

//chrono stuff
boolean isTimerRunning = false;
double startTime, endTime, lastStartTime;
const float FT_TO_CM = 0.23622;

//button stuff
byte toggleModeIncrementButtonState = 0, toggleModeLastIncrementButtonState = 0, toggleModeDecrementButtonState = 0, toggleModeLastDecrementButtonState = 0, mode = 1; 

//ammo counter stuff
const byte MAX_AMMO = 18;
byte currentAmmo = 18;
boolean isDartThrough = false;

//change mag stuff
byte magReleaseState = 0, magReleaseLastState = 0;
boolean isMagIn = false;

void setup() {
  Serial.begin(9600);

  //initialize display stuff
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    
  pinMode(TOGGLE_MODE_INCREMENT_BUTTON_PIN, INPUT);
  pinMode(TOGGLE_MODE_DECREMENT_BUTTON_PIN, INPUT);
  pinMode(MAG_RELEASE_SWITCH_PIN, INPUT);
}

void loop() {
  //toggle mode button stuff
  toggleModeIncrementButtonState = digitalRead(TOGGLE_MODE_INCREMENT_BUTTON_PIN);
  if (toggleModeLastIncrementButtonState != toggleModeIncrementButtonState && toggleModeIncrementButtonState == HIGH) {
    mode++;
    checkMode(mode);
  }
  toggleModeLastIncrementButtonState = toggleModeIncrementButtonState;

  toggleModeDecrementButtonState = digitalRead(TOGGLE_MODE_DECREMENT_BUTTON_PIN);
  if (toggleModeLastDecrementButtonState != toggleModeDecrementButtonState && toggleModeDecrementButtonState == HIGH) {
    mode--;
    checkMode(mode);
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
  }
  lastPhotoState = readPhotoSensor;

  //volt meter stuff
  if (mode % 4 == 3) {
    voltMeter(analogRead(VOLT_METER_PIN));
  }
  
  magReleaseState = digitalRead(MAG_RELEASE_SWITCH_PIN);
//  changeMag();
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

void voltMeter (int value) {
  float r2Val = 100000.0, r1Val = 10000.0;
  float voltageOut = (value * 5.0) / 1024.0;
  float voltageIn = voltageOut / (r1Val/ (r2Val + r1Val));
  
  //display voltage
  displayText((String)voltageIn, 3);
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

void checkMode(byte modeVal) {
  if (modeVal % 4 == 0) {
    displayText("chrono");
  } else if (modeVal % 4 == 1) {
    displayText("ammo counter");
  } else if (modeVal % 4 == 2) {
    displayText("rate of fire");
  } else if (modeVal % 4 == 3) {
    displayText("volt meter");
  }
}

void displayText(String text) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor( (SCREEN_WIDTH/2) - (text.length()*2), SCREEN_HEIGHT - 10);
  display.print(text);
  display.display();
}

void displayText(String text, int textSize) {
  display.clearDisplay();
  display.setTextSize(textSize);
  display.setTextColor(WHITE);
  display.setCursor( (SCREEN_WIDTH/2) - ((text.length()*2)* (textSize * 1.5)), 0);
  display.print(text);
  display.display();
}

