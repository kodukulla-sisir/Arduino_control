#include "MotorControl.h"
#include <Arduino.h>

MotorControl::MotorControl(int dir_pin, int step_pin, int enable_pin) {
  _dirPin = dir_pin;
  _stepPin = step_pin;
  _enablePin = enable_pin;
  _targetRPM = 0;
  _currentRPM = 0;
  _isEnabled = false;
  _lastRampTime = 0;
  _lastStepTime = 0;
  _stepState = LOW;
}

void MotorControl::begin() {
  pinMode(_dirPin, OUTPUT);
  pinMode(_stepPin, OUTPUT);
  pinMode(_enablePin, OUTPUT);
  
  digitalWrite(_dirPin, HIGH);
  digitalWrite(_enablePin, HIGH); // HIGH = Disabled on A4988
}

void MotorControl::setTargetRPM(int rpm) {
  _targetRPM = abs(rpm); // positive val for rpm
  // Set direction based on sign
  digitalWrite(_dirPin, (rpm >= 0) ? HIGH : LOW);
}

void MotorControl::setMotorEnabled(bool enabled) {
  _isEnabled = enabled;
  if (!enabled) {
    digitalWrite(_enablePin, HIGH); // Disable
    _targetRPM = 0;
    _currentRPM = 0; // Immediate stop
  } else {
    digitalWrite(_enablePin, LOW); // Enable
  }
}

int MotorControl::getCurrentRPM() { return _currentRPM; }
bool MotorControl::isEnabled() { return _isEnabled; }

unsigned long MotorControl::rpmToMicrosDelay(int rpmValue) {
  if (rpmValue == 0) return 0;
  
  return 60000000UL / ((unsigned long)rpmValue * 1600UL);
}

void MotorControl::update() {
  unsigned long currentMillis = millis();
  unsigned long currentMicros = micros();

  // 1. Process Ramping
  if (currentMillis - _lastRampTime >= _rampInterval) {
    _lastRampTime = currentMillis;
    if (_currentRPM < _targetRPM) {
      _currentRPM += _rpmStep;
      if (_currentRPM > _targetRPM) _currentRPM = _targetRPM;
    } else if (_currentRPM > _targetRPM) {
      _currentRPM -= _rpmStep;
      if (_currentRPM < _targetRPM) _currentRPM = _targetRPM;
    }
  }

  // 2. Process Stepping (Non-blocking)
  if (_isEnabled && _currentRPM > 0) {
    unsigned long stepInterval = rpmToMicrosDelay(_currentRPM);
    
    if (currentMicros - _lastStepTime >= stepInterval) {
      _lastStepTime = currentMicros;
      
      digitalWrite(_stepPin, HIGH);
      delayMicroseconds(2); 
      digitalWrite(_stepPin, LOW);
    }
  }
}