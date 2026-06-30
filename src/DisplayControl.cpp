#include "DisplayControl.h"

DisplayControl::DisplayControl(int cs, int dc, int rst) 
  : _tft(cs, dc, rst) {
  _lastRPM = -1;
  _lastState = false;
  _forceRedraw = true;
}

void DisplayControl::begin() {
  _tft.begin();
  _tft.setRotation(1);
  _tft.fillScreen(ILI9341_BLACK);
  
  _tft.setTextColor(ILI9341_WHITE);
  _tft.setTextSize(2);
  _tft.setCursor(10, 20);
  _tft.print("Controller Status");
  
  _tft.setCursor(10, 80);
  _tft.print("Motor: ");
  
  _tft.setCursor(10, 140);
  _tft.print("Speed: ");
}

void DisplayControl::updateScreen(int currentRPM, bool isEnabled) {
  // Display updates only if there is a change
  if (_lastRPM == currentRPM && _lastState == isEnabled && !_forceRedraw) {
    return; 
  }

  // Erase previous value
  _tft.fillRect(100, 80, 150, 30, ILI9341_BLACK); 
  _tft.fillRect(100, 140, 150, 30, ILI9341_BLACK);
  _tft.setTextSize(3);

  // Redraw
  _tft.setCursor(100, 80);
  if (isEnabled) {
    _tft.setTextColor(ILI9341_GREEN);
    _tft.print("ON");
  } else {
    _tft.setTextColor(ILI9341_RED);
    _tft.print("OFF");
  }

  // Redraw Speed
  _tft.setCursor(100, 140);
  _tft.setTextColor(ILI9341_CYAN);
  _tft.print(currentRPM); 
  _tft.print(" RPM");

  _lastRPM = currentRPM;
  _lastState = isEnabled;
  _forceRedraw = false;
}