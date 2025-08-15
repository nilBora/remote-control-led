#include <Wire.h>
#include "RTClib.h"
#include <TinyIRSender.hpp>
#include <avr/sleep.h>

RTC_DS3231 rtc;

const int IR_MY_SEND_PIN = 3;
const int DS3231_INT_PIN = 2; // Пін, куди підключено INT/SQW з DS3231

volatile bool wakeUpFlag = false;

void wakeUp() {
  wakeUpFlag = true;
}

void setup() {
  Serial.begin(9600);
  Wire.begin();

  pinMode(DS3231_INT_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(DS3231_INT_PIN), wakeUp, FALLING);

  if (!rtc.begin()) {
    Serial.println("DS3231 not found");
    while (1);
  }

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, setting time to compile time");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  // Вимикаємо Square Wave
  rtc.writeSqwPinMode(DS3231_OFF);

  // Очищаємо старі будильники
  rtc.clearAlarm(1);
  rtc.clearAlarm(2);

  // Встановлюємо перший будильник на 07:00:00
  setAlarm(7, 0, 0);

  Serial.println("System ready, going to sleep...");
}

void loop() {
  if (!wakeUpFlag) {
    goToSleep();
  }

  wakeUpFlag = false;

  DateTime now = rtc.now();

  Serial.print("Wake up at: ");
  Serial.print(now.hour());
  Serial.print(":");
  Serial.print(now.minute());
  Serial.print(":");
  Serial.println(now.second());

  // Відправка IR сигналу
  sendLedSignal();

  // Програмуємо наступний будильник
  if (now.hour() < 20) {
    setAlarm(20, 0, 0);
  } else {
    setAlarm(7, 0, 0);
  }

  Serial.println("Next alarm set, sleeping again...");
}

void sendLedSignal() {
  Serial.println("Sending LED ON/OFF signal...");
  sendNEC(IR_MY_SEND_PIN, 0x00, 0x40, 0);
}

void setAlarm(byte hour, byte minute, byte second) {
  // Очищаємо попередній будильник
  rtc.clearAlarm(1);

  // Записуємо новий будильник (Alarm1, match H:M:S)
  Wire.beginTransmission(0x68);
  Wire.write(0x07); // Регістр Alarm1, секунди
  Wire.write(decToBcd(second));
  Wire.write(decToBcd(minute));
  Wire.write(decToBcd(hour));
  Wire.write(0x80); // Мatch тільки годину/хвилину/секунду
  Wire.endTransmission();

  // Увімкнути Alarm1
  Wire.beginTransmission(0x68);
  Wire.write(0x0E); // Регістр Control
  Wire.write(0b00000101); // INT enabled + Alarm1 enabled
  Wire.endTransmission();
}

byte decToBcd(byte val) {
  return ((val / 10 * 16) + (val % 10));
}

void goToSleep() {
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  sleep_cpu();
  sleep_disable();
}