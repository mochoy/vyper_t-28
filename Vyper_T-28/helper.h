void voltMeter(int);
void displayText(String);
void displayText(String, int);

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

void chrono () {
  const float FT_TO_CM = 0.23622;
  
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

void doMode() {
  if (mode % 4 == 0) {
    chrono();
    displayText("chrono");
  } else if (mode % 4 == 1) {
    ammoCounter();
    displayText("ammo counter");
  } else if (mode % 4 == 2) {
    rateOfFire();
    displayText("rate of fire");
  } else if (mode % 4 == 3) {
    voltMeter(analogRead(VOLT_METER_PIN));
    displayText("volt meter");
  }
}

void voltMeter (int value) {
  float r2Val = 100000.0, r1Val = 10000.0;
  float voltageOut = (value * 5.0) / 1024.0;
  float voltageIn = voltageOut / (r1Val/ (r2Val + r1Val));
  
  //display voltage
  displayText((String)voltageIn, 3);
}

void displayText(String text) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor( (SCREEN_WIDTH/2) - ((text.length()*2) * (1 * 1.5)), SCREEN_HEIGHT - 10);
  display.print(text);
  display.display();
}

void displayText(String text, int textSize) {
  display.clearDisplay();
  display.setTextSize(textSize);
  display.setTextColor(WHITE);
  display.setCursor( (SCREEN_WIDTH/2) - ((text.length()*2) * (textSize * 1.5)), 0);
  display.print(text);
  display.display();
}

