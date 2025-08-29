#include <SPI.h>
#include <MFRC522.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>

#define SS_PIN 5    // GPIO 5 cho MFRC522 SDA
#define RST_PIN 22  // GPIO 22 cho MFRC522 RST
#define TFT_CS 15   // GPIO 15 cho ILI9341 CS
#define TFT_DC 2    // GPIO 2 cho ILI9341 DC
#define TFT_RST 4   // GPIO 4 cho ILI9341 RST
#define SPI_MOSI 23 // VSPI_MOSI
#define SPI_MISO 19 // VSPI_MISO
#define SPI_SCK 18  // VSPI_SCK
#define MAX_UID_LENGTH 7

MFRC522 rfid(SS_PIN, RST_PIN);
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);

enum State {
  CHECK_IN,
  MAIN_MENU,
  CHECK_OUT
};
State currentState = CHECK_IN;

byte currentUID[MAX_UID_LENGTH];
byte uidLength = 0;
bool rfidCheckedIn = false;

void printWrapped(String text, int x, int y, uint16_t color, uint8_t textSize) {
  tft.setTextColor(color);
  tft.setTextSize(textSize);
  int maxChars = (textSize == 1) ? 53 : 26;
  int lineHeight = (textSize == 1) ? 12 : 20;
  int currentY = y;

  while (text.length() > 0) {
    String line = text.substring(0, maxChars);
    if (text.length() > maxChars) {
      int lastSpace = line.lastIndexOf(' ');
      if (lastSpace > 0 && lastSpace < maxChars) {
        line = text.substring(0, lastSpace);
        text = text.substring(lastSpace + 1);
      } else {
        text = text.substring(maxChars);
      }
    } else {
      text = "";
    }
    tft.setCursor(x, currentY);
    tft.println(line);
    currentY += lineHeight;
    if (currentY >= 240) break;
  }
}

void setup() {
  Serial.begin(115200);
  SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);
  rfid.PCD_Init();
  tft.begin();
  tft.setRotation(1);
  tft.fillScreen(ILI9341_BLACK);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(1);
  tft.setCursor(0, 0);
  tft.println("=== Check in ===");
  tft.println("Quet the RFID de dang nhap");
  Serial.println("=== Check in ===");
  Serial.println("Quét thẻ RFID để đăng nhập");
}

String byteArrayToString(byte *uid, byte uidLength) {
  String uidStr = "";
  for (byte i = 0; i < uidLength; i++) {
    if (uid[i] < 0x10) uidStr += "0";
    uidStr += String(uid[i], HEX);
  }
  uidStr.toUpperCase();
  return uidStr;
}

bool readRFID(byte *uid, byte *uidLength) {
  unsigned long startTime = millis();
  while (millis() - startTime < 30000) {
    if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
      *uidLength = rfid.uid.size;
      if (*uidLength > MAX_UID_LENGTH) *uidLength = MAX_UID_LENGTH;
      for (byte i = 0; i < *uidLength; i++) {
        uid[i] = rfid.uid.uidByte[i];
      }
      rfid.PICC_HaltA();
      rfid.PCD_StopCrypto1();
      return true;
    }
  }
  return false;
}

void showMainMenu() {
  tft.fillRect(0, 0, 320, 240, ILI9341_BLACK);
  tft.setTextSize(1);
  tft.setCursor(0, 0);
  printWrapped("Da dang nhap. UID: " + byteArrayToString(currentUID, uidLength), 0, 0, ILI9341_WHITE, 1);
  tft.setCursor(0, 24);
  tft.println("Quet the RFID de check out");
  Serial.println("Đã đăng nhập. UID: " + byteArrayToString(currentUID, uidLength));
  Serial.println("Quét thẻ RFID để check out");
}

void loop() {
  byte uid[MAX_UID_LENGTH];
  byte length;

  switch (currentState) {
    case CHECK_IN:
      tft.fillRect(0, 0, 320, 240, ILI9341_BLACK);
      tft.setCursor(0, 0);
      tft.setTextColor(ILI9341_YELLOW);
      tft.setTextSize(2);
      tft.println("Dang quet the...");
      Serial.println("Đang quét thẻ...");
      if (readRFID(uid, &length)) {
        memcpy(currentUID, uid, length);
        uidLength = length;
        rfidCheckedIn = true;
        currentState = MAIN_MENU;
        showMainMenu();
      } else {
        tft.fillRect(0, 0, 320, 240, ILI9341_BLACK);
        tft.setCursor(0, 0);
        tft.setTextColor(ILI9341_MAGENTA);
        tft.setTextSize(1);
        tft.println("Khong doc duoc the...");
        tft.println("Quet lai the RFID");
        Serial.println("Không đọc được thẻ...");
        Serial.println("Quét lại thẻ RFID");
        delay(2000);
        tft.fillRect(0, 0, 320, 240, ILI9341_BLACK);
        tft.setTextColor(ILI9341_WHITE);
        tft.setTextSize(1);
        tft.setCursor(0, 0);
        tft.println("=== Check in ===");
        tft.println("Quet the RFID de dang nhap");
      }
      break;

    case MAIN_MENU:
      if (readRFID(uid, &length)) {
        tft.fillRect(0, 0, 320, 240, ILI9341_BLACK);
        tft.setCursor(0, 0);
        tft.setTextColor(ILI9341_YELLOW);
        tft.setTextSize(2);
        tft.println("Dang quet the...");
        Serial.println("Đang quét thẻ...");
        if (length == uidLength && memcmp(uid, currentUID, length) == 0) {
          tft.fillRect(0, 0, 320, 240, ILI9341_BLACK);
          tft.setCursor(0, 0);
          tft.setTextColor(ILI9341_GREEN);
          tft.setTextSize(2);
          tft.println("Check out thanh cong");
          Serial.println("Check out thành công");
          rfidCheckedIn = false;
          currentState = CHECK_IN;
          delay(2000);
          tft.fillRect(0, 0, 320, 240, ILI9341_BLACK);
          tft.setTextColor(ILI9341_WHITE);
          tft.setTextSize(1);
          tft.setCursor(0, 0);
          tft.println("=== Check in ===");
          tft.println("Quet the RFID de dang nhap");
          Serial.println("=== Check in ===");
          Serial.println("Quét thẻ RFID để đăng nhập");
        } else {
          tft.fillRect(0, 0, 320, 240, ILI9341_BLACK);
          tft.setCursor(0, 0);
          tft.setTextColor(ILI9341_MAGENTA);
          tft.setTextSize(1);
          tft.println("The khong khop voi the check-in!");
          tft.println("Quet lai the RFID");
          Serial.println("Thẻ không khớp với thẻ check-in!");
          Serial.println("Quét lại thẻ RFID");
          delay(2000);
          showMainMenu();
        }
      }
      break;
  }
}
