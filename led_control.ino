#include <Wire.h>
#include "RTClib.h"
#include <TinyIRSender.hpp>
#include <avr/sleep.h>

RTC_DS3231 rtc;

const int IR_MY_SEND_PIN = 3;
const int DS3231_INT_PIN = 2; // INT/SQW від DS3231

// Гнучкі налаштування часу
byte morningHour = 7;
byte morningMinute = 0;
byte eveningHour = 20;
byte eveningMinute = 52; // приклад — 20:20

volatile bool woke = false;

void isrWake() {
  woke = true;
}

static inline byte decToBcd(byte v) { return (v/10)*16 + (v%10); }

void setAlarmDaily(byte hour, byte minute, byte second) {
  rtc.clearAlarm(1);

  Wire.beginTransmission(0x68);
  Wire.write(0x07);                         
  Wire.write(decToBcd(second) & 0x7F);      
  Wire.write(decToBcd(minute) & 0x7F);      
  Wire.write(decToBcd(hour)   & 0x7F);      
  Wire.write(0x80);                         
  Wire.endTransmission();

  Wire.beginTransmission(0x68);
  Wire.write(0x0E);               
  Wire.write(0b00000101);         
  Wire.endTransmission();
}

void goToSleep() {
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  sleep_cpu();
  sleep_disable();
}

void sendLedSignal() {
  Serial.println("IR: send 0x40");
  sendNEC(IR_MY_SEND_PIN, 0x00, 0x40, 0);
}

void setup() {
  Serial.begin(115200);
  delay(100);
  Serial.println("READY");
  delay(500);
  Wire.begin();

  pinMode(DS3231_INT_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(DS3231_INT_PIN), isrWake, LOW);

  Serial.println("Init RTC");
  delay(100);
  
  // Scan I2C devices
  Serial.println("Scanning I2C...");
  for (byte i = 8; i < 120; i++) {
    Wire.beginTransmission(i);
    if (Wire.endTransmission() == 0) {
      Serial.print("Found device at 0x");
      Serial.println(i, HEX);
    }
  }
  
  if (!rtc.begin()) {
    Serial.println("DS3231 not found");
    while (1);
  }
  Serial.println("RTC initialized!");
  if (rtc.lostPower()) {
    Serial.println("RTC lost power -> set compile time");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  rtc.writeSqwPinMode(DS3231_OFF);
  rtc.clearAlarm(1);
  rtc.clearAlarm(2);

  DateTime now = rtc.now();
  Serial.print("Current RTC time: ");
  Serial.print(now.hour()); Serial.print(":");
  Serial.print(now.minute()); Serial.print(":");
  Serial.println(now.second());

  // Вибираємо найближчий час
  if (now.hour() < morningHour || 
     (now.hour() == morningHour && now.minute() < morningMinute)) {
    setAlarmDaily(morningHour, morningMinute, 0);
    Serial.print("First alarm set for: ");
    Serial.print(morningHour); Serial.print(":"); Serial.println(morningMinute);
  } 
  else if (now.hour() < eveningHour || 
           (now.hour() == eveningHour && now.minute() < eveningMinute)) {
    setAlarmDaily(eveningHour, eveningMinute, 0);
    Serial.print("First alarm set for: ");
    Serial.print(eveningHour); Serial.print(":"); Serial.println(eveningMinute);
  } 
  else {
    setAlarmDaily(morningHour, morningMinute, 0);
    Serial.print("First alarm set for: ");
    Serial.print(morningHour); Serial.print(":"); Serial.println(morningMinute);
  }

  Serial.println("System ready -> going to sleep...");
  delay(1000);
  goToSleep();
}

void loop() {
  if (!woke) return;
  woke = false;

  rtc.clearAlarm(1);

  DateTime now = rtc.now();
  Serial.print("Woke at "); Serial.print(now.hour());
  Serial.print(':'); Serial.print(now.minute());
  Serial.print(':'); Serial.println(now.second());

  sendLedSignal();

  if (now.hour() < eveningHour || 
     (now.hour() == eveningHour && now.minute() < eveningMinute)) {
    setAlarmDaily(eveningHour, eveningMinute, 0);
    Serial.print("Next alarm set for: ");
    Serial.print(eveningHour); Serial.print(":"); Serial.println(eveningMinute);
  } else {
    setAlarmDaily(morningHour, morningMinute, 0);
    Serial.print("Next alarm set for: ");
    Serial.print(morningHour); Serial.print(":"); Serial.println(morningMinute);
  }

  Serial.println("Going back to sleep...");
  goToSleep();
}
