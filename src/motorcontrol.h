#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H

#include <Arduino.h>

class MotorControl {
  public:
    MotorControl(int dir_pin, int step_pin, int enable_pin);
    
    void begin();
    void update(); 
    void setTargetRPM(int rpm);
    void setMotorEnabled(bool enabled);
    
    int getCurrentRPM();
    bool isEnabled();

  private:
    int _dirPin, _stepPin, _enablePin;
    int _targetRPM, _currentRPM;
    bool _isEnabled;
    
    // Ramping variables
    unsigned long _lastRampTime;
    const unsigned long _rampInterval = 50; // ms
    const int _rpmStep = 5;
    
    // Non-blocking step variables
    unsigned long _lastStepTime;
    bool _stepState;
    const int _stepsPerRev = 400; // Change if using microstepping
    
    unsigned long rpmToMicrosDelay(int rpmValue);
};

#endif