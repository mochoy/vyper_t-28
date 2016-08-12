void displayText(String);
void displayText(String, int);

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

