#include <TinyIRSender.hpp>
#include <microDS3231.h>

MicroDS3231 rtc;

const int IR_MY_SEND_PIN = 3;
void setup() {
  Serial.begin(9600);
  // проверка наличия модуля на линии i2c
  // вызов rtc.begin() не обязателен для работы
  if (!rtc.begin()) {
    Serial.println("DS3231 not found");
    for(;;);
  }
  
  // ======== УСТАНОВКА ВРЕМЕНИ АВТОМАТИЧЕСКИ ========
   rtc.setTime(COMPILE_TIME);     // установить время == времени компиляции
  
    
  if (rtc.lostPower()) {
    Serial.println("lost power!");
  }

}

void loop() {
  byte hour = rtc.getHours();
  byte minute = rtc.getMinutes();
  byte second = rtc.getSeconds();

  Serial.print("Time: ");
  Serial.print(hour);
  Serial.print(":");
  Serial.print(minute);
  Serial.print(":");
  Serial.println(second);

  // О 07:00:00
  if (hour == 7 && minute == 0 && second == 0) {
    sendLedSignal();
  }

  // О 20:00:00
  if (hour == 20 && minute == 0 && second == 0) {
    sendLedSignal();
  }

  delay(1000); // Оновлення кожну секунду
}

void sendLedSignal() {
  Serial.println("Sending LED ON/OFF signal...");
  sendNEC(IR_MY_SEND_PIN, 0x00, 0x40, 0); // Адреса 0x00, команда 0x40
}