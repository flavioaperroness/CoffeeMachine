#include <Servo.h>
#include <Wire.h>
#include <RTClib.h>
#include "Arduino_SensorKit.h"


const uint8_t  SERVO_PIN          = 9;
const uint8_t  BREW_HOUR          = 6; // Time valve should open
const uint8_t  BREW_MINUTE        = 30;
const uint8_t  VALVE_CLOSED_ANGLE = 0;     
const uint8_t  VALVE_OPEN_ANGLE   = 90;  
const uint32_t VALVE_OPEN_MS      = 8000;  // Open for 8 seconds
const uint16_t LOOP_DELAY_MS      = 500;

const char* const MONTHS[] = {"JAN", "FEB", "MAR", "APR", "MAY", "JUN",
                              "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"};


Servo valveServo;
RTC_DS3231 rtc;
int8_t lastBrewDay = -1; // Prevents the valve from opening twice in one day 

// Display
void showScreen(const DateTime& now) {
  char dateLine[10];
  snprintf(dateLine, sizeof(dateLine), "%s %02d", MONTHS[now.month() - 1], now.day());

  Oled.clearDisplay();
  Oled.setFont(u8x8_font_7x14B_1x2_r);
  Oled.setCursor(0, 0);
  Oled.print("Buon Caffe");
  Oled.setCursor(0, 4);
  Oled.print(dateLine);
  Oled.refreshDisplay();
}

// Coffee machine valve
void runValve() {
  valveServo.attach(SERVO_PIN);
  valveServo.write(VALVE_OPEN_ANGLE);
  delay(VALVE_OPEN_MS);
  valveServo.write(VALVE_CLOSED_ANGLE);
  delay(600);  // Gives the servo time to return   
  valveServo.detach();    // Saves power  
}

bool isBrewTime(const DateTime& now) {
  return now.hour() == BREW_HOUR &&
         now.minute() == BREW_MINUTE &&
         now.day() != lastBrewDay;
}

void setup() {
  Oled.begin();
  Oled.setFlipMode(true);

  if (!rtc.begin()) {
    Oled.setFont(u8x8_font_7x14B_1x2_r);
    Oled.print("CLOCK ERROR");
    Oled.refreshDisplay();
    while (true) {}
  }
}

void loop() {
  DateTime now = rtc.now();
  showScreen(now);

  if (isBrewTime(now)) {
    lastBrewDay = now.day();
    runValve();
  }
  delay(LOOP_DELAY_MS);
}
