#include <Arduino.h>
#include "MotorControl.h"
#include "DisplayControl.h"

MotorControl stepper(2, 3, 8); // DIR, STEP, ENABLE
DisplayControl display(10, 9, 8); // CS, DC, RST

void setup() {
  Serial.begin(115200); // From ROS bridge
  Serial.setTimeout(5); 

  stepper.begin();
  display.begin();
}

void loop() {
  // 1. Read the parsed target RPM from the ROS 2 Python script
  if (Serial.available() > 0) {
    int targetRPM = Serial.parseInt();

    // Debug echo back to Terminal 4
    Serial.print("RECEIVED RPM: ");
    Serial.println(targetRPM);

    // Clear any leftover junk in the buffer
    while(Serial.available() > 0) {
      Serial.read();
    }

    // 2. Apply the target to the motor
    if (targetRPM == 0) {
      stepper.setMotorEnabled(false);
    } else {
      stepper.setMotorEnabled(true);
      stepper.setTargetRPM(targetRPM);
    }
  }

  // 3. Keep the motor stepping (CRITICAL: This must be active)
  stepper.update(); 

  // 4. Update the screen
  display.updateScreen(stepper.getCurrentRPM(), stepper.isEnabled());
}