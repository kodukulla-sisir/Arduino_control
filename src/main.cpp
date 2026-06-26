#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>

// Motor Pins
const int DIR_PIN = 2;
const int STEP_PIN = 3;

// Display pins
const int TFT_CS = 10;
const int TFT_DC = 9;
const int TFT_RST = 8;

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);

// variables
bool motor_is_on = false;
float motor_speed_hz = 1.0;         
const float HZ_INCREMENT = 0.1;      // speed increment
const float MAX_HZ = 10.0;          // Max speed 
const float MIN_HZ = 0.5;            // Min speed 

int last_button_state = 0;
unsigned long last_joystick_read_time = 0;
const int JOYSTICK_COOLDOWN_MS = 150; // Speed control to stop rapid increments

unsigned long last_step_time = 0;
bool step_pin_state = LOW;

bool display_needs_update = true;

void setup() {
  Serial.begin(115200);
  Serial.setTimeout(5); 
  
  pinMode(DIR_PIN, OUTPUT);
  pinMode(STEP_PIN, OUTPUT);
  digitalWrite(DIR_PIN, HIGH); 

  tft.begin();
  tft.setRotation(1); 
  tft.fillScreen(ILI9341_BLACK);
  
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(2);
  tft.setCursor(10, 20);
  tft.print("Micro-Robot Control");
  
  tft.setCursor(10, 80);
  tft.print("Status: ");
  
  tft.setCursor(10, 140);
  tft.print("Speed: ");
}

void updateDisplay() {
  tft.fillRect(100, 80, 150, 30, ILI9341_BLACK); 
  tft.fillRect(100, 140, 150, 30, ILI9341_BLACK); // clears display

  tft.setTextSize(3);

  tft.setCursor(100, 80);
  if (motor_is_on) {
    tft.setTextColor(ILI9341_GREEN);
    tft.print("ON");
  } else {
    tft.setTextColor(ILI9341_RED);
    tft.print("OFF");
  }

  tft.setCursor(100, 140);
  tft.setTextColor(ILI9341_CYAN);
  tft.print(motor_speed_hz, 2); // updates motor speed
  tft.print(" Hz");
}

void loop() {
  unsigned long current_millis = millis();
  unsigned long current_micros = micros();

  if (Serial.available() > 0) {
    // string format from bridge: "-1.00,1\n"
    String incoming_str = Serial.readStringUntil(',');
    float joy_y = incoming_str.toFloat();
    
    int btn = Serial.readStringUntil('\n').toInt();

    //toggles motor when button is pressed
    if (btn == 1 && last_button_state == 0) {
      motor_is_on = !motor_is_on;
      display_needs_update = true;
    }
    last_button_state = btn;

    if (current_millis - last_joystick_read_time > JOYSTICK_COOLDOWN_MS) {
      if (joy_y > 0.5 && motor_speed_hz < MAX_HZ) {
        motor_speed_hz += HZ_INCREMENT;
        display_needs_update = true;
        last_joystick_read_time = current_millis;
      } 
      else if (joy_y < -0.5 && motor_speed_hz > MIN_HZ) {
        motor_speed_hz -= HZ_INCREMENT;
        display_needs_update = true;
        last_joystick_read_time = current_millis;
      }
    }
  }

  if (motor_is_on) {
    // Calculate microseconds per step toggle (1 Hz = 1 full cycle of HIGH then LOW)
    unsigned long toggle_interval = 1000000UL / (motor_speed_hz * 2);

    if (current_micros - last_step_time >= toggle_interval) {
      last_step_time = current_micros;
      step_pin_state = !step_pin_state;
      digitalWrite(STEP_PIN, step_pin_state);
    }
  } else {
    digitalWrite(STEP_PIN, LOW); // Ensure pin rests LOW when off
  }

  
  if (display_needs_update) {
    updateDisplay();
    display_needs_update = false;
  }
}