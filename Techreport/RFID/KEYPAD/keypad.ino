#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <Keypad.h>

#define TFT_CS 15   // GPIO 15 cho ILI9341 CS
#define TFT_DC 2    // GPIO 2 cho ILI9341 DC
#define TFT_RST 4   // GPIO 4 cho ILI9341 RST

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);

const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {13, 14, 12, 27};
byte colPins[COLS] = {26, 25, 33, 32};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

enum State {
  MAIN_MENU,
  WORK_MENU,
  TASK_MENU,
  WORKING
};
State currentState = MAIN_MENU;

struct Task {
  int completed = 0;
  int failed = 0;
  bool available = true;
};
Task lapRap, suaChua, dongGoi;
Task* currentTask = nullptr;
const char* currentTaskName = nullptr;

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
  tft.begin();
  tft.setRotation(1);
  tft.fillScreen(ILI9341_BLACK);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(1);
  tft.setCursor(0, 0);
  tft.println("=== Menu chinh ===");
  tft.println("1. Bat dau lam viec");
  Serial.println("=== Menu chính ===");
  Serial.println("1. Bắt đầu làm việc");
}

void showMainMenu() {
  tft.fillRect(0, 0, 320, 240, ILI9341_BLACK);
  tft.setTextSize(1);
  tft.setCursor(0, 0);
  tft.println("=== Menu chinh ===");
  tft.println("1. Bat dau lam viec");
  Serial.println("=== Menu chính ===");
  Serial.println("1. Bắt đầu làm việc");
}

void showWorkMenu() {
  tft.fillRect(0, 0, 320, 240, ILI9341_BLACK);
  tft.setTextSize(2);
  tft.setCursor(0, 0);
  tft.setTextColor(ILI9341_WHITE);
  tft.println("=== Chon cong viec ===");
  tft.setTextSize(1);
  tft.setCursor(0, 20);
  if (lapRap.available) tft.println("1. Lap rap");
  tft.setCursor(0, 32);
  if (suaChua.available) tft.println("2. Sua chua");
  tft.setCursor(0, 44);
  if (dongGoi.available) tft.println("3. Dong goi");
  tft.setCursor(0, 56);
  tft.println("*. Quay lai");
  Serial.println("=== Chọn công việc ===");
  if (lapRap.available) Serial.println("1. Lắp ráp");
  if (suaChua.available) Serial.println("2. Sửa chữa");
  if (dongGoi.available) Serial.println("3. Đóng gói");
  Serial.println("*. Quay lại");
}

void showTaskMenu(const char* taskName) {
  tft.fillRect(0, 0, 320, 240, ILI9341_BLACK);
  tft.setTextSize(2);
  tft.setCursor(0, 0);
  tft.setTextColor(ILI9341_WHITE);
  String title = String("=== ") + taskName + " ===";
  printWrapped(title, 0, 0, ILI9341_WHITE, 2);
  tft.setTextSize(1);
  tft.setCursor(0, 24);
  tft.println("1. Bat dau lam viec");
  tft.setCursor(0, 36);
  tft.println("2. Xem so cong viec hoan thanh");
  tft.setCursor(0, 48);
  tft.println("3. Xem so loi");
  tft.setCursor(0, 60);
  tft.println("*. Quay lai");
  Serial.print("=== "); Serial.print(taskName); Serial.println(" ===");
  Serial.println("1. Bắt đầu làm việc");
  Serial.println("2. Xem số công việc hoàn thành");
  Serial.println("3. Xem số lỗi");
  Serial.println("*. Quay lại");
}

void showWorkingMenu(const char* taskName) {
  tft.fillRect(0, 0, 320, 240, ILI9341_BLACK);
  tft.setTextSize(2);
  tft.setCursor(0, 0);
  tft.setTextColor(ILI9341_WHITE);
  String title = String("=== ") + taskName + " - Lam viec ===";
  printWrapped(title, 0, 0, ILI9341_WHITE, 2);
  tft.setTextSize(1);
  tft.setCursor(0, 24);
  tft.println("1. Hoan thanh");
  tft.setCursor(0, 36);
  tft.println("2. Loi");
  tft.setCursor(0, 48);
  tft.println("*. Quay lai");
  Serial.print("=== "); Serial.print(taskName); Serial.println(" - Làm việc ===");
  Serial.println("1. Hoàn thành");
  Serial.println("2. Lỗi");
  Serial.println("*. Quay lại");
}

void loop() {
  char key = keypad.getKey();
  if (!key) return;

  switch (currentState) {
    case MAIN_MENU:
      switch (key) {
        case '1':
          currentState = WORK_MENU;
          showWorkMenu();
          break;
        default:
          tft.fillRect(0, 0, 320, 240, ILI9341_BLACK);
          tft.setCursor(0, 0);
          tft.setTextColor(ILI9341_MAGENTA);
          tft.setTextSize(1);
          tft.println("Phim khong hop le!");
          showMainMenu();
          Serial.println("Phím không hợp lệ!");
          break;
      }
      break;

    case WORK_MENU:
      switch (key) {
        case '1':
          if (lapRap.available) {
            currentTask = &lapRap;
            currentTaskName = "Lap rap";
            currentState = TASK_MENU;
            showTaskMenu(currentTaskName);
          } else {
            tft.fillRect(0, 0, 320, 240, ILI9341_BLACK);
            tft.setCursor(0, 0);
            tft.setTextColor(ILI9341_MAGENTA);
            tft.setTextSize(1);
            tft.println("Cong viec khong kha dung!");
            showWorkMenu();
            Serial.println("Công việc không khả dụng!");
          }
          break;
        case '2':
          if (suaChua.available) {
            currentTask = &suaChua;
            currentTaskName = "Sua chua";
            currentState = TASK_MENU;
            showTaskMenu(currentTaskName);
          } else {
            tft.fillRect(0, 0, 320, 240, ILI9341_BLACK);
            tft.setCursor(0, 0);
            tft.setTextColor(ILI9341_MAGENTA);
            tft.setTextSize(1);
            tft.println("Cong viec khong kha dung!");
            showWorkMenu();
            Serial.println("Công việc không khả dụng!");
          }
          break;
        case '3':
          if (dongGoi.available) {
            currentTask = &dongGoi;
            currentTaskName = "Dong goi";
            currentState = TASK_MENU;
            showTaskMenu(currentTaskName);
          } else {
            tft.fillRect(0, 0, 320, 240, ILI9341_BLACK);
            tft.setCursor(0, 0);
            tft.setTextColor(ILI9341_MAGENTA);
            tft.setTextSize(1);
            tft.println("Cong viec khong kha dung!");
            showWorkMenu();
            Serial.println("Công việc không khả dụng!");
          }
          break;
        case '*':
          currentState = MAIN_MENU;
          showMainMenu();
          break;
        default:
          tft.fillRect(0, 0, 320, 240, ILI9341_BLACK);
          tft.setCursor(0, 0);
          tft.setTextColor(ILI9341_MAGENTA);
          tft.setTextSize(1);
          tft.println("Phim khong hop le!");
          showWorkMenu();
          Serial.println("Phím không hợp lệ!");
          break;
      }
      break;

    case TASK_MENU:
      switch (key) {
        case '1':
          currentState = WORKING;
          showWorkingMenu(currentTaskName);
          break;
        case '2':
          tft.fillRect(0, 0, 320, 240, ILI9341_BLACK);
          tft.setCursor(0, 0);
          tft.setTextColor(ILI9341_CYAN);
          tft.setTextSize(1);
          printWrapped(String(currentTaskName) + ": Hoan thanh: " + String(currentTask->completed), 0, 0, ILI9341_CYAN, 1);
          tft.setCursor(0, 24);
          tft.println("Nhan bat ky phim de quay lai");
          Serial.print(currentTaskName); Serial.print(": Hoàn thành: "); Serial.println(currentTask->completed);
          Serial.println("Nhấn bất kỳ phím để quay lại");
          break;
        case '3':
          tft.fillRect(0, 0, 320, 240, ILI9341_BLACK);
          tft.setCursor(0, 0);
          tft.setTextColor(ILI9341_MAGENTA);
          tft.setTextSize(1);
          printWrapped(String(currentTaskName) + ": Loi: " + String(currentTask->failed), 0, 0, ILI9341_MAGENTA, 1);
          tft.setCursor(0, 24);
          tft.println("Nhan bat ky phim de quay lai");
          Serial.print(currentTaskName); Serial.print(": Lỗi: "); Serial.println(currentTask->failed);
          Serial.println("Nhấn bất kỳ phím để quay lại");
          break;
        case '*':
          currentState = MAIN_MENU;
          showMainMenu();
          break;
        default:
          tft.fillRect(0, 0, 320, 240, ILI9341_BLACK);
          tft.setCursor(0, 0);
          tft.setTextColor(ILI9341_MAGENTA);
          tft.setTextSize(1);
          tft.println("Phim khong hop le!");
          showTaskMenu(currentTaskName);
          Serial.println("Phím không hợp lệ!");
          break;
      }
      break;

    case WORKING:
      switch (key) {
        case '1':
          if (currentTask) {
            currentTask->completed++;
            tft.fillRect(0, 0, 320, 240, ILI9341_BLACK);
            tft.setCursor(0, 0);
            tft.setTextColor(ILI9341_GREEN);
            tft.setTextSize(2);
            String completedMsg = "Da hoan thanh 1 SP - " + String(currentTaskName);
            printWrapped(completedMsg, 0, 0, ILI9341_GREEN, 2);
            showWorkingMenu(currentTaskName);
            Serial.print("Đã hoàn thành 1 sản phẩm - "); Serial.println(currentTaskName);
          }
          break;
        case '2':
          if (currentTask) {
            currentTask->failed++;
            tft.fillRect(0, 0, 320, 240, ILI9341_BLACK);
            tft.setCursor(0, 0);
            tft.setTextColor(ILI9341_MAGENTA);
            tft.setTextSize(2);
            String failedMsg = "Da ghi nhan 1 loi - " + String(currentTaskName);
            printWrapped(failedMsg, 0, 0, ILI9341_MAGENTA, 2);
            showWorkingMenu(currentTaskName);
            Serial.print("Đã ghi nhận 1 lỗi - "); Serial.println(currentTaskName);
          }
          break;
        case '*':
          currentState = WORK_MENU;
          showWorkMenu();
          break;
        default:
          tft.fillRect(0, 0, 320, 240, ILI9341_BLACK);
          tft.setCursor(0, 0);
          tft.setTextColor(ILI9341_MAGENTA);
          tft.setTextSize(1);
          tft.println("Phim khong hop le!");
          showWorkingMenu(currentTaskName);
          Serial.println("Phím không hợp lệ!");
          break;
      }
      break;
  }
}
