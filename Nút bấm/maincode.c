#include <Arduino.h>

// Khai báo chân nút
#define BTN_UP    13
#define BTN_DOWN  14
#define BTN_ENTER 27

// Mảng lưu thông tin nút
const int buttonPins[] = {BTN_UP, BTN_DOWN, BTN_ENTER};
char buttonNames[] = {'U', 'D', 'E'};

// Biến chống dội
int lastButtonState[3] = {HIGH, HIGH, HIGH};
int buttonState[3]     = {HIGH, HIGH, HIGH};
unsigned long lastDebounceTime[3] = {0, 0, 0};
const unsigned long debounceDelay = 50;

// Hàm setup
void setup() {
  Serial.begin(9600);
  for (int i = 0; i < 3; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
  }
}

// Hàm xử lý nút nhấn
void handleButtonPress(char btn) {
  Serial.print("Button pressed: ");
  Serial.println(btn);
  // Ở đây bạn có thể nối vào FSM CHECK_IN / MENU ... giống code gốc
}

// Hàm đọc nút
void updateButtons() {
  for (int i = 0; i < 3; i++) {
    int reading = digitalRead(buttonPins[i]);

    if (reading != lastButtonState[i]) {
      lastDebounceTime[i] = millis();
    }

    if ((millis() - lastDebounceTime[i]) > debounceDelay) {
      if (reading != buttonState[i]) {
        buttonState[i] = reading;
        if (buttonState[i] == LOW) {
          handleButtonPress(buttonNames[i]);
        }
      }
    }
    lastButtonState[i] = reading;
  }
}

// Vòng lặp chính
void loop() {
  updateButtons();
}
