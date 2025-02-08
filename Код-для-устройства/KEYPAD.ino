#include <Keypad.h>
#include <Servo.h>

// Настройки клавиатуры
const byte ROWS = 4;
const byte COLS = 3;
char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};
byte rowPins[ROWS] = {5, 4, 3, 2};
byte colPins[COLS] = {8, 7, 6};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Компоненты
Servo servo;
const int servoPin = 9;
const int buzzerPin = 10;
const int irSensorPin = A0;

// Пин-код
const char secretCode[4] = {'1','4','6','4'};
char enteredCode[4];
int codeIndex = 0;

// ИК датчик
int irThreshold = 450;        // Пороговое значение сырого сигнала
bool servoActive = false;
bool obstacleWasDetected = false; // Флаг обнаружения препятствия
unsigned long returnTime = 0;

void setup() {
  Serial.begin(9600);
  servo.attach(servoPin);
  pinMode(buzzerPin, OUTPUT);
  servo.write(0);
}

void loop() {
  char key = keypad.getKey();
  Serial.println(27.86*pow((analogRead(irSensorPin)*0.0048828125),-1.15));
  // Обработка клавиатуры
  if (key) {
    if (key == '#') {
      codeIndex = 0;
      playTone(500, 100);
    } 
    else if (key != '*' && codeIndex < 4) {
      playTone(1000, 50);
      enteredCode[codeIndex] = key;
      codeIndex++;
      
      if (codeIndex == 4) {
        if (checkCode()) {
          playSuccessSound();
          servo.write(80);
          servoActive = true;
        } else {
          playErrorSound();
        }
        codeIndex = 0;
      }
    }
  }

  // Логика ИК датчика
  if (servoActive) {
    int irValue = analogRead(irSensorPin);
    bool currentObstacle = (irValue > irThreshold);

    // Обнаружение нового препятствия
    if (currentObstacle) {
      obstacleWasDetected = true; // Запоминаем факт обнаружения
    }

    // Если препятствие исчезло после обнаружения
    if (!currentObstacle && obstacleWasDetected) {
      returnTime = millis() + 2000; // Запуск таймера на 2 секунды
      obstacleWasDetected = false;  // Сброс флага
    }

    // Возврат сервы после исчезновения препятствия
    if (returnTime != 0 && millis() > returnTime) {
      servo.write(0);
      servoActive = false;
      returnTime = 0;
    }
  }
}

bool checkCode() {
  for (int i = 0; i < 4; i++) {
    if (enteredCode[i] != secretCode[i]) return false;
  }
  return true;
}

void playTone(int freq, int duration) {
  tone(buzzerPin, freq, duration);
  delay(duration);
}

void playSuccessSound() {
  playTone(1500, 100);
  delay(50);
  playTone(2000, 100);
}

void playErrorSound() {
  playTone(300, 300);
}