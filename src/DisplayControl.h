#ifndef DISPLAY_CONTROL_H
#define DISPLAY_CONTROL_H

#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>

class DisplayControl {
  public:
    DisplayControl(int cs, int dc, int rst);
    void begin();
    void updateScreen(int currentRPM, bool isEnabled);

  private:
    Adafruit_ILI9341 _tft;
    int _lastRPM;
    bool _lastState;
    bool _forceRedraw;
};

#endif