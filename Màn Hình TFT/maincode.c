#include <Arduino.h>
#include <Adafruit_GFX.h>      // Thư viện đồ họa cơ bản
#include <Adafruit_ILI9341.h>  // Thư viện cho TFT ILI9341

// ================== KHAI BÁO CHÂN TFT ==================
#define TFT_CS   15   // Chip Select
#define TFT_DC   2    // Data/Command
#define TFT_RST  4    // Reset
// ESP32 SPI hardware pins:
// MOSI = 23, MISO = 19, SCK = 18

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);

// ================== SETUP ==================
void setup() {
  Serial.begin(9600);

  // Khởi động TFT
  tft.begin();
  tft.setRotation(1); // Xoay ngang (1 = landscape)

  // Xóa màn hình, tô nền
  tft.fillScreen(ILI9341_BLACK);

  // In chữ test
  tft.setCursor(20, 30);
  tft.setTextColor(ILI9341_GREEN);
  tft.setTextSize(2);
  tft.println("Test TFT ILI9341");

 case MAIN_MENU:
      if (button == 'U') {
        menuIndex = (menuIndex == 0) ? maxMenuItems - 1 : menuIndex - 1;
        showMainMenu();
      } else if (button == 'D') {
        menuIndex = (menuIndex == maxMenuItems - 1) ? 0 : menuIndex + 1;
        showMainMenu();
      } else if (button == 'E') {
        if (menuIndex == 0) {
          currentState = WORK_MENU;
          menuIndex = 0;
          maxMenuItems = 4; // Menu công việc có 4 lựa chọn (3 công việc + quay lại)
          showWorkMenu();
        } else if (menuIndex == 1) {
          tft.fillScreen(ILI9341_BLACK);
          tft.setCursor(0, 0);
          tft.setTextColor(ILI9341_YELLOW);
          tft.println("Dang check out...");
          tft.println("Nhan Enter de quet the");
          Serial.println("Đang check out...");
          Serial.println("Nhấn Enter để quét thẻ");
          currentState = CHECK_OUT;
          showSummaryDisplayed = false;
        }
      }
      break;

    case WORK_MENU:
      if (button == 'U') {
        menuIndex = (menuIndex == 0) ? maxMenuItems - 1 : menuIndex - 1;
        showWorkMenu();
      } else if (button == 'D') {
        menuIndex = (menuIndex == maxMenuItems - 1) ? 0 : menuIndex + 1;
        showWorkMenu();
      } else if (button == 'E') {
        if (menuIndex == 0) {
          currentTask = &lapRap;
          currentTaskName = "Lap rap";
          tft.fillScreen(ILI9341_BLACK);
          tft.setCursor(0, 0);
          tft.setTextColor(ILI9341_GREEN);
          tft.println("=== Lap rap ===");
          Serial.println("=== Lắp ráp ===");
          currentState = TASK_MENU;
          menuIndex = 0;
          maxMenuItems = 4; // Menu công việc có 4 lựa chọn
          showTaskMenu(currentTaskName);
        } else if (menuIndex == 1) {
          currentTask = &suaChua;
          currentTaskName = "Sua chua";
          tft.fillScreen(ILI9341_BLACK);
          tft.setCursor(0, 0);
          tft.setTextColor(ILI9341_GREEN);
          tft.println("=== Sua chua ===");
          Serial.println("=== Sửa chữa ===");
          currentState = TASK_MENU;
          menuIndex = 0;
          maxMenuItems = 4;
          showTaskMenu(currentTaskName);
        } else if (menuIndex == 2) {
          currentTask = &dongGoi;
          currentTaskName = "Dong goi";
          tft.fillScreen(ILI9341_BLACK);
          tft.setCursor(0, 0);
          tft.setTextColor(ILI9341_GREEN);
          tft.println("=== Dong goi ===");
          Serial.println("=== Đóng gói ===");
          currentState = TASK_MENU;
          menuIndex = 0;
          maxMenuItems = 4;
          showTaskMenu(currentTaskName);
        } else if (menuIndex == 3) {
          currentState = MAIN_MENU;
          menuIndex = 0;
          maxMenuItems = 2;
          showMainMenu();
        }
      }
      break;

    case TASK_MENU:
      if (button == 'U') {
        menuIndex = (menuIndex == 0) ? maxMenuItems - 1 : menuIndex - 1;
        showTaskMenu(currentTaskName);
      } else if (button == 'D') {
        menuIndex = (menuIndex == maxMenuItems - 1) ? 0 : menuIndex + 1;
        showTaskMenu(currentTaskName);
      } else if (button == 'E') {
        if (menuIndex == 0) {
          currentState = WORKING;
          menuIndex = 0;
          maxMenuItems = 3; // Menu làm việc có 3 lựa chọn
          showWorkingMenu(currentTaskName);
        } else if (menuIndex == 1) {
          tft.fillScreen(ILI9341_BLACK);
          tft.setCursor(0, 0);
          tft.setTextColor(ILI9341_CYAN);
          tft.print(currentTaskName);
          tft.print(": Hoan thanh: ");
          tft.println(currentTask->hoanThanh);
          Serial.print(currentTaskName);
          Serial.print(": Hoàn thành: ");
          Serial.println(currentTask->hoanThanh);
          delay(2000); // Hiển thị trong 2 giây
          showTaskMenu(currentTaskName);
        } else if (menuIndex == 2) {
          tft.fillScreen(ILI9341_BLACK);
          tft.setCursor(0, 0);
          tft.setTextColor(ILI9341_CYAN);
          tft.print(currentTaskName);
          tft.print(": Loi: ");
          tft.println(currentTask->loi);
          Serial.print(currentTaskName);
          Serial.print(": Lỗi: ");
          Serial.println(currentTask->loi);
          delay(2000); // Hiển thị trong 2 giây
          showTaskMenu(currentTaskName);
        } else if (menuIndex == 3) {
          currentState = MAIN_MENU;
          menuIndex = 0;
          maxMenuItems = 2;
          showMainMenu();
        }
      }
      break;

    case WORKING:
      if (button == 'U') {
        menuIndex = (menuIndex == 0) ? maxMenuItems - 1 : menuIndex - 1;
        showWorkingMenu(currentTaskName);
      } else if (button == 'D') {
        menuIndex = (menuIndex == maxMenuItems - 1) ? 0 : menuIndex + 1;
        showWorkingMenu(currentTaskName);
      } else if (button == 'E') {
        if (menuIndex == 0) {
          if (currentTask) {
            currentTask->hoanThanh++;
            tft.fillScreen(ILI9341_BLACK);
            tft.setCursor(0, 0);
            tft.setTextColor(ILI9341_GREEN);
            tft.print("Da hoan thanh 1 SP - ");
            tft.println(currentTaskName);
            Serial.print("Đã hoàn thành 1 sản phẩm - ");
            Serial.println(currentTaskName);
            delay(1000); // Hiển thị trong 1 giây
            showWorkingMenu(currentTaskName);
          }
        } else if (menuIndex == 1) {
          if (currentTask) {
            currentTask->loi++;
            tft.fillScreen(ILI9341_BLACK);
            tft.setCursor(0, 0);
            tft.setTextColor(ILI9341_RED);
            tft.print("Da ghi nhan 1 loi - ");
            tft.println(currentTaskName);
            Serial.print("Đã ghi nhận 1 lỗi - ");
            Serial.println(currentTaskName);
            delay(1000); // Hiển thị trong 1 giây
            showWorkingMenu(currentTaskName);
          }
        } else if (menuIndex == 2) {
          currentState = WORK_MENU;
          menuIndex = 0;
          maxMenuItems = 4;
          showWorkMenu();
        }
      }
      break;

    case CHECK_OUT:
      if (!showSummaryDisplayed) {
        if (button == 'E') {
          tft.fillScreen(ILI9341_BLACK);
          tft.setCursor(0, 0);
          tft.setTextColor(ILI9341_YELLOW);
          tft.println("Dang quet the de check out...");
          Serial.println("Đang quét thẻ để check out...");
          if (readRFID()) {
            bool match = true;
            for (byte i = 0; i < rfid.uid.size; i++) {
              if (rfid.uid.uidByte[i] != currentUID[i]) {
                match = false;
                break;
              }
            }
            if (match) {
              // --- NEW: gửi POST checkout (completed=0, failed=0) ---
              String uidStr = currentUidToString();
              Serial.println("Check-out UID: " + uidStr);
              sendPostRequest(uidStr, "checkout", 0, 0);

              tft.fillScreen(ILI9341_BLACK);
              tft.setCursor(0, 0);
              tft.setTextColor(ILI9341_GREEN);
              tft.println("Check out thanh cong");
              Serial.println("Check out thành công");
              showSummary();
              tft.setTextColor(ILI9341_WHITE);
              tft.println("Nhan Enter de tiep tuc");
              Serial.println("Nhấn Enter để tiếp tục");
              showSummaryDisplayed = true;

              // reset flags
              hasCheckedInForCurrentUID = false;
              lastScannedUID = "";
            } else {
              tft.fillScreen(ILI9341_BLACK);
              tft.setCursor(0, 0);
              tft.setTextColor(ILI9341_RED);
              tft.println("The khong khop voi the check-in!");
              tft.println("Nhan Enter de quet lai");
              Serial.println("Thẻ không khớp với thẻ check-in!");
              Serial.println("Nhấn Enter để quét lại");
            }
          } else {
            tft.fillScreen(ILI9341_BLACK);
            tft.setCursor(0, 0);
            tft.setTextColor(ILI9341_RED);
            tft.println("Khong doc duoc the:");
            tft.println("Het thoi gian hoac loi doc.");
            tft.println("Nhan Enter de quet lai");
            Serial.println("Không đọc được thẻ: Hết thời gian hoặc lỗi đọc.");
            Serial.println("Nhấn Enter để quét lại");
          }
        }
      } 