#include <Arduino.h>
#include "MotorControl.h"
#include "DisplayControl.h"

MotorControl stepper(2, 3, 8); // DIR, STEP, ENABLE
DisplayControl display(10, 9, 8); // CS, DC, RST

int lastButtonState = 0;
const int MAX_RPM = 500;
const int RPM_BUMP = 50;

void setup() {
  Serial.begin(115200); // From ROS bridge
  Serial.setTimeout(5); 

  stepper.begin();
  display.begin();
}

void loop() {
  // Read controller data
  if (Serial.available() > 0) {
    String incomingStr = Serial.readStringUntil(',');
    float joyY = incomingStr.toFloat();
    int btn = Serial.readStringUntil('\n').toInt();

    //debug
    Serial.print("RECEIVED - Joystick: ");
    Serial.print(joyY);
    Serial.print(", Button: ");
    Serial.println(btn);

    // Toggle on/off
    if (btn == 1 && lastButtonState == 0) {
      stepper.setMotorEnabled(!stepper.isEnabled());
    }
    lastButtonState = btn;

    // convert joystick data to RPM
    if (stepper.isEnabled()) {
      int target = joyY * MAX_RPM;
      
      // deadzone
      if (abs(joyY) < 0.15) target = 0; 
      
      stepper.setTargetRPM(target);
    }
  }

  //stepper.update(); // Must be called in loop

  // Display update
  display.updateScreen(stepper.getCurrentRPM(), stepper.isEnabled());
}