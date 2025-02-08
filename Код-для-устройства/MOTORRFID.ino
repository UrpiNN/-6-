#include <SPI.h>
#include <MFRC522.h>
#include <Stepper.h>

//РФИД
#define RST_PIN 9
#define SS_PIN 10

//ПЬЕЗОЗУМЕР
const int buzzerPin = 8;

//ШАГОВЫЙ
#define IN1 2
#define IN2 3
#define IN3 4
#define IN4 5
const int STEPS_PER_REVOLUTION = 2048; // Полный оборот (в полушаговом режиме)
const float DEGREES_PER_STEP = 360.0 / STEPS_PER_REVOLUTION; // ≈ 0.17578°
const int STEPS_FOR_50_DEGREES = round(120 / DEGREES_PER_STEP); // ≈ 284 шага
Stepper myStepper(STEPS_PER_REVOLUTION, IN1, IN3, IN2, IN4); // Порядок пинов важен!

MFRC522 mfrc522(SS_PIN, RST_PIN);

byte authorizedUid[] = {0x4E, 0x21, 0x57, 0x6D};

void setup() {
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();
  myStepper.setSpeed(10); // Скорость вращения (оборотов в минуту)

}

void loop() {
  // Проверка новой метки
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  // Считывание метки
  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  // Вывод информации о метке
  Serial.print("UID метки: ");
  dumpByteArray(mfrc522.uid.uidByte, mfrc522.uid.size);
  Serial.println();
  
  bool isMatch = true;
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    if (mfrc522.uid.uidByte[i] != authorizedUid[i]) {
      isMatch = false;
      break;
    }
  }
  
  if (isMatch) {
    Serial.println("Доступ разрешен!");
    tone(1500, 100);
    delay(50);
    tone(2000, 100);
    myStepper.step(STEPS_FOR_50_DEGREES);
  delay(5000);
  myStepper.step(0-STEPS_FOR_50_DEGREES);
  } else {
    Serial.println("Доступ запрещен!");
    tone(300, 300);
  }



  // Остановка работы с меткой
  mfrc522.PICC_HaltA();
  }


  void dumpByteArray(byte *buffer, byte bufferSize) {
    for (byte i = 0; i < bufferSize; i++) {
      Serial.print(buffer[i] < 0x10 ? " 0" : " ");
      Serial.print(buffer[i], HEX);
  }
}
