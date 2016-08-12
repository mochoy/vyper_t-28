void voltMeter(int);
void displayModeText();
void displayText(String, int);

void rateOfFire () {
  if (currentAmmo >= 0) {   //make sure still in rate of fire mode and there are still darts
    if ((readPhotoSensor > HIGH_VAL_1) && !isDartThrough) {   //if dart passes
      isDartThrough = true;
    } else if ((readPhotoSensor < HIGH_VAL_1) && isDartThrough) {   //if dart passes
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
  if ((readPhotoSensor > HIGH_VAL_1) && !isDartThrough) {   //if laser not shining
    isDartThrough = true;
  } else if ((readPhotoSensor < HIGH_VAL_1) && isDartThrough) {
    isDartThrough = false;
    if (currentAmmo > 0) {
      currentAmmo --;
      displayText((String)currentAmmo, 4);
    } 
  }

  Serial.println(readPhotoSensor);
}

void chrono () {
  const float FT_TO_CM = 0.23622;
  
  if ((readPhotoSensor > HIGH_VAL_1) && !isTimerRunning) {   //if laser not shining
    isTimerRunning = true;
    startTime = micros();    //"start timer"
  } else if ((readPhotoSensor < HIGH_VAL_1) && isTimerRunning)  {  //if laser shining
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
  if (mode == 0) {
    chrono();
    displayModeText();
    digitalWrite(LASER_1_PIN, HIGH);
    digitalWrite(LASER_2_PIN, HIGH);
  } else if (mode == 1) {
    ammoCounter();
    displayModeText();
    digitalWrite(LASER_1_PIN, HIGH);
    digitalWrite(LASER_2_PIN, LOW);
  } else if (mode == 2) {
    rateOfFire();
    displayModeText();
    digitalWrite(LASER_1_PIN, HIGH);
    digitalWrite(LASER_2_PIN, LOW);
  } else if (mode == 3) {
    voltMeter(analogRead(VOLT_METER_PIN));
    displayModeText();
    digitalWrite(LASER_1_PIN, LOW);
    digitalWrite(LASER_2_PIN, LOW);
  }
}

void voltMeter (int value) {
  float r2Val = 100000.0, r1Val = 10000.0;
  float voltageOut = (value * 5.0) / 1024.0;
  float voltageIn = voltageOut / (r1Val/ (r2Val + r1Val));

  if (voltageIn < 0.1) {
    voltageIn = 0; 
  }
  
  displayText((String)voltageIn, 3);
}

void displayModeText() {
  String text;
  if (mode == 0) {
    text = "CHRONOGRAPH";
  } else if (mode == 1) {
    text = "AMMO COUNTER";    
  } else if (mode == 2) {
    text = "RATE OF FIRE";
  } else if (mode == 3) {
    text = "VOLT METER";
  }
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor( (SCREEN_WIDTH/2) - ((text.length()*2) * (1 * 1.5)), SCREEN_HEIGHT - 10);
  display.print(text);
  display.display();
}

void displayText(String text, int textSize) {
  displayModeText();
	
  display.setTextSize(textSize);
  display.setTextColor(WHITE);
  display.setCursor( (SCREEN_WIDTH/2) - ((text.length()*2) * (textSize * 1.5)), 0);
  display.print(text);
  display.display();
}

