// ========== Thêm thư viện mạng ==========
#include <WiFi.h>
#include <HTTPClient.h>

// ========== Phần cấu hình WiFi / WebApp ==========
const char* WIFI_SSID = "YOUR_WIFI_SSID";
const char* WIFI_PASS = "YOUR_WIFI_PASSWORD";
const char* serverUrl = "YOUR_WEB_APP_URL"; // Ví dụ: "https://script.google.com/macros/s/AKfycb.../exec"

// ========== Thư viện và cấu hình phần cứng gốc của bạn ==========
#include <SPI.h>
#include <MFRC522.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>

#define SS_PIN 5     // GPIO 5 cho MFRC522 SDA
#define RST_PIN 22   // GPIO 22 cho MFRC522 RST
#define RFID_TIMEOUT 30000 // Thời gian chờ quét RFID: 30 giây
#define TFT_CS 15    // GPIO 15 cho ILI9341 CS
#define TFT_DC 2     // GPIO 2 cho ILI9341 DC
#define TFT_RST 4    // GPIO 4 cho ILI9341 RST

// Nút bấm (giữ nguyên chân như trong code test)
#define BTN_UP 13    // GPIO 13 cho nút Up
#define BTN_DOWN 14  // GPIO 14 cho nút Down
#define BTN_ENTER 27 // GPIO 27 cho nút Enter

// Sử dụng VSPI mặc định của ESP32
#define SPI_MOSI 23  // VSPI_MOSI
#define SPI_MISO 19  // VSPI_MISO
#define SPI_SCK 18   // VSPI_SCK

MFRC522 rfid(SS_PIN, RST_PIN);
byte currentUID[4]; // Lưu UID của thẻ (giả định UID 4 byte)
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);

enum State {
  CHECK_IN,
  MAIN_MENU,
  WORK_MENU,
  TASK_MENU,
  WORKING,
  CHECK_OUT
};

State currentState = CHECK_IN;

// Biến theo dõi công việc
struct Task {
  int hoanThanh;
  int loi;
};

Task lapRap = {0, 0};
Task suaChua = {0, 0};
Task dongGoi = {0, 0};
Task* currentTask = nullptr; // Con trỏ đến công việc hiện tại
const char* currentTaskName = nullptr; // Tên công việc hiện tại
bool rfidCheckedIn = false;
bool showSummaryDisplayed = false; // Theo dõi trạng thái hiển thị tổng quan

// Biến cho menu
int menuIndex = 0;
int maxMenuItems = 0;

// Biến cho nút bấm (sử dụng từ code test)
unsigned long lastDebounceTime[3] = {0, 0, 0};
bool lastButtonState[3] = {HIGH, HIGH, HIGH}; 
bool buttonState[3] = {HIGH, HIGH, HIGH};
const unsigned long debounceDelay = 50; // 50ms chống dội

unsigned long lastActivityTime = 0;

// Biến bổ sung cho logic GET/POST
String lastScannedUID = ""; // để tránh xử lý lặp UID liên tiếp
bool hasCheckedInForCurrentUID = false; // đánh dấu nếu đã check-in cho currentUID

// =================== Hàm hỗ trợ mạng ===================
void connectWiFi() {
  Serial.print("Connecting to WiFi ");
  Serial.print(WIFI_SSID);
  Serial.print(" ... ");
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 15000) {
    delay(300);
    Serial.print(".");
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected. IP: " + WiFi.localIP().toString());
  } else {
    Serial.println("\nWiFi connection failed (timeout). Continue without network.");
  }
}

// Gửi GET lấy thông tin nhân viên (in payload ra Serial, có thể parse nếu cần)
void getEmployeeInfo(const String &uid) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("GET skipped: WiFi not connected");
    return;
  }
  HTTPClient http;
  String url = String(serverUrl) + "?vID=" + uid;
  http.begin(url);
  int code = http.GET();
  if (code > 0) {
    String payload = http.getString();
    Serial.println("GET " + url + " => " + String(code));
    Serial.println("Payload: " + payload);

    // Hiển thị tóm tắt lên TFT (giữ nguyên style của bạn)
    tft.fillScreen(ILI9341_BLACK);
    tft.setCursor(0, 0);
    tft.setTextColor(ILI9341_WHITE);
    tft.setTextSize(2);
    tft.println("Nhan vien:");
    tft.setTextSize(1);
    tft.println(payload); // payload có thể dài — bạn có thể parse JSON nếu muốn hiển thị đẹp hơn
  } else {
    Serial.println("GET failed, code: " + String(code));
  }
  http.end();
}

// Gửi POST checkin / checkout (completed, failed hiện đặt = 0)
void sendPostRequest(const String &uid, const String &type, int completed, int failed) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("POST skipped: WiFi not connected");
    return;
  }
  HTTPClient http;
  http.begin(serverUrl);
  http.addHeader("Content-Type", "application/json");

  String json = "{\"vID\"😕"" + uid + "\",\"type\"😕"" + type + "\",\"completed\":" + String(completed) + ",\"failed\":" + String(failed) + "}";
  int code = http.POST(json);
  if (code > 0) {
    String resp = http.getString();
    Serial.println("POST (" + type + ") code=" + String(code) + " resp=" + resp);
  } else {
    Serial.println("POST failed, code=" + String(code));
  }
  http.end();
}

// Helper: chuyển mảng currentUID[] -> String hex (giống hiện hiển ở Serial)
String currentUidToString() {
  String s = "";
  for (byte i = 0; i < 4; i++) {
    if (currentUID[i] < 0x10) s += "0";
    s += String(currentUID[i], HEX);
  }
  s.toUpperCase();
  return s;
}

// =================== setup() ===================
void setup() {
  Serial.begin(9600);

  // Khởi tạo SPI với VSPI (giữ nguyên)
  SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);

  // Khởi tạo nút bấm (giống code test)
  pinMode(BTN_UP, INPUT_PULLUP);
  pinMode(BTN_DOWN, INPUT_PULLUP);
  pinMode(BTN_ENTER, INPUT_PULLUP);

  rfid.PCD_Init();
  tft.begin();
  tft.setRotation(1); // Xoay màn hình ngang
  tft.fillScreen(ILI9341_BLACK); // Xóa màn hình
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(2);
  tft.setCursor(0, 0);
  tft.println("=== Check in ===");
  tft.println("Nhan Enter de quet the");
  Serial.println("=== Check in ===");
  Serial.println("Nhấn Enter để quét thẻ");

  lastActivityTime = millis();

  // Kết nối WiFi (nếu bạn muốn có ngay)
  connectWiFi();
}

// =================== loop() và các hàm xử lý nút ===================
void loop() {
  // Đọc nút bấm sử dụng hàm từ code test
  readButton(0, BTN_UP, "UP");
  readButton(1, BTN_DOWN, "DOWN");
  readButton(2, BTN_ENTER, "ENTER");

  // Kiểm tra thời gian không hoạt động
  if (millis() - lastActivityTime > 60000 && currentState != CHECK_IN && currentState != CHECK_OUT) {
    currentState = MAIN_MENU;
    menuIndex = 0;
    maxMenuItems = 2;
    showMainMenu();
    lastActivityTime = millis();
  }
}

// Hàm đọc nút bấm từ code test (giữ nguyên)
void readButton(uint8_t index, uint8_t pin, const char* name) {
  bool reading = digitalRead(pin);

  if (reading != lastButtonState[index]) {
    lastDebounceTime[index] = millis();
  }

  if ((millis() - lastDebounceTime[index]) > debounceDelay) {
    if (reading != buttonState[index]) {
      buttonState[index] = reading;
      if (buttonState[index] == LOW) { // Nút được nhấn
        Serial.printf("Button %s pressed\n", name);
        handleButtonPress(name[0]); // Gửi ký tự đầu tiên của tên nút ('U', 'D', 'E')
        lastActivityTime = millis();
      }
    }
  }
  lastButtonState[index] = reading;
}

// =================== handleButtonPress (giữ nguyên) ===================
void handleButtonPress(char button) {
  switch (currentState) {
    case CHECK_IN:
      if (button == 'E') {
        tft.fillScreen(ILI9341_BLACK);
        tft.setCursor(0, 0);
        tft.setTextColor(ILI9341_YELLOW);
        tft.println("Dang quet the...");
        Serial.println("Đang quét thẻ...");
        if (readRFID()) {
          // --- NEW: sau khi đọc thẻ thành công (check-in flow) ---
          String uidStr = currentUidToString();
          Serial.println("Read UID (check-in): " + uidStr);

          // Gửi GET để lấy thông tin hiển thị
          getEmployeeInfo(uidStr);

          // Gửi POST type = checkin (completed = 0, failed = 0)
          sendPostRequest(uidStr, "checkin", 0, 0);

          // --- giữ nguyên phần chuyển trạng thái ---
          rfidCheckedIn = true;
          currentState = MAIN_MENU;
          menuIndex = 0;
          maxMenuItems = 2; // Menu chính có 2 lựa chọn
          showMainMenu();

          // lưu uid hiện tại để dùng khi check-out
          lastScannedUID = uidStr;
          hasCheckedInForCurrentUID = true;
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
      break;

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
      } else {
        if (button == 'E') {
          rfidCheckedIn = false;
          currentTask = nullptr;
          currentTaskName = nullptr;
          lapRap.hoanThanh = 0; lapRap.loi = 0;
          suaChua.hoanThanh = 0; suaChua.loi = 0;
          dongGoi.hoanThanh = 0; dongGoi.loi = 0;
          currentState = CHECK_IN;
          showSummaryDisplayed = false;
          tft.fillScreen(ILI9341_BLACK);
          tft.setCursor(0, 0);
          tft.setTextColor(ILI9341_WHITE);
          tft.println("=== Check in ===");
          tft.println("Nhan Enter de quet the");
          Serial.println("=== Check in ===");
          Serial.println("Nhấn Enter để quét thẻ");
        }
      }
      break;
  }
}

// =================== Các hàm hiển thị (giữ nguyên) ===================
void showMainMenu() {
  tft.fillScreen(ILI9341_BLACK);
  tft.setCursor(0, 0);
  tft.setTextColor(ILI9341_WHITE);
  tft.println("=== MENU ===");
  tft.print("UID: ");
  for (byte i = 0; i < 4; i++) {
    tft.print(currentUID[i], HEX);
    tft.print(" ");
  }
  tft.println();
  
  // Hiển thị menu với item được chọn
  tft.setTextColor(menuIndex == 0 ? ILI9341_YELLOW : ILI9341_WHITE);
  tft.println("1. Lam viec");
  tft.setTextColor(menuIndex == 1 ? ILI9341_YELLOW : ILI9341_WHITE);
  tft.println("2. Check out");
  
  Serial.println("=== MENU ===");
  Serial.print("UID: ");
  for (byte i = 0; i < 4; i++) {
    Serial.print(currentUID[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
  Serial.println("1. Làm việc");
  Serial.println("2. Check out");
}

void showWorkMenu() {
  tft.fillScreen(ILI9341_BLACK);
  tft.setCursor(0, 0);
  tft.setTextColor(ILI9341_WHITE);
  tft.println("=== Chon cong viec ===");
  
  // Hiển thị menu với item được chọn
  tft.setTextColor(menuIndex == 0 ? ILI9341_YELLOW : ILI9341_WHITE);
  tft.println("1. Lap rap");
  tft.setTextColor(menuIndex == 1 ? ILI9341_YELLOW : ILI9341_WHITE);
  tft.println("2. Sua chua");
  tft.setTextColor(menuIndex == 2 ? ILI9341_YELLOW : ILI9341_WHITE);
  tft.println("3. Dong goi");
  tft.setTextColor(menuIndex == 3 ? ILI9341_YELLOW : ILI9341_WHITE);
  tft.println("*. Quay lai menu chinh");
  
  Serial.println("=== Chọn công việc ===");
  Serial.println("1. Lắp ráp");
  Serial.println("2. Sửa chữa");
  Serial.println("3. Đóng gói");
  Serial.println("*. Quay lại menu chính");
}

void showTaskMenu(const char* taskName) {
  tft.fillScreen(ILI9341_BLACK);
  tft.setCursor(0, 0);
  tft.setTextColor(ILI9341_WHITE);
  tft.print("=== "); tft.print(taskName); tft.println(" ===");
  
  // Hiển thị menu với item được chọn
  tft.setTextColor(menuIndex == 0 ? ILI9341_YELLOW : ILI9341_WHITE);
  tft.println("1. Bat dau lam viec");
  tft.setTextColor(menuIndex == 1 ? ILI9341_YELLOW : ILI9341_WHITE);
  tft.println("2. Xem so cong viec hoan thanh");
  tft.setTextColor(menuIndex == 2 ? ILI9341_YELLOW : ILI9341_WHITE);
  tft.println("3. Xem so loi");
  tft.setTextColor(menuIndex == 3 ? ILI9341_YELLOW : ILI9341_WHITE);
  tft.println("*. Quay lai menu chinh");
  
  Serial.print("=== "); Serial.print(taskName); Serial.println(" ===");
  Serial.println("1. Bắt đầu làm việc");
  Serial.println("2. Xem số công việc hoàn thành");
  Serial.println("3. Xem số lỗi");
  Serial.println("*. Quay lại menu chính");
}

void showWorkingMenu(const char* taskName) {
  tft.fillScreen(ILI9341_BLACK);
  tft.setCursor(0, 0);
  tft.setTextColor(ILI9341_WHITE);
  tft.print("=== "); tft.print(taskName); tft.println(" - Lam viec ===");
  
  // Hiển thị menu với item được chọn
  tft.setTextColor(menuIndex == 0 ? ILI9341_YELLOW : ILI9341_WHITE);
  tft.println("1. Hoan thanh");
  tft.setTextColor(menuIndex == 1 ? ILI9341_YELLOW : ILI9341_WHITE);
  tft.println("2. Loi");
  tft.setTextColor(menuIndex == 2 ? ILI9341_YELLOW : ILI9341_WHITE);
  tft.println("*. Quay lai menu cong viec");
  
  Serial.print("=== "); Serial.print(taskName); Serial.println(" - Làm việc ===");
  Serial.println("1. Hoàn thành");
  Serial.println("2. Lỗi");
  Serial.println("*. Quay lại menu công việc");
}

void showSummary() {
  tft.fillScreen(ILI9341_BLACK);
  tft.setCursor(0, 0);
  tft.setTextColor(ILI9341_CYAN);
  tft.println("=== Tong quan cong viec ===");
  tft.println();
  tft.print("Lap rap - Hoan thanh: "); tft.print(lapRap.hoanThanh); tft.print(", Loi: "); tft.println(lapRap.loi);
  tft.print("Sua chua - Hoan thanh: "); tft.print(suaChua.hoanThanh); tft.print(", Loi: "); tft.println(suaChua.loi);
  tft.print("Dong goi - Hoan thanh: "); tft.print(dongGoi.hoanThanh); tft.print(", Loi: "); tft.println(dongGoi.loi);
  tft.println();
  Serial.println("=== Tổng quan công việc ===");
  Serial.print("Lắp ráp - Hoàn thành: "); Serial.print(lapRap.hoanThanh); Serial.print(", Lỗi: "); Serial.println(lapRap.loi);
  Serial.print("Sửa chữa - Hoàn thành: "); Serial.print(suaChua.hoanThanh); Serial.print(", Lỗi: "); Serial.println(suaChua.loi);
  Serial.print("Đóng gói - Hoàn thành: "); Serial.print(dongGoi.hoanThanh); Serial.print(", Lỗi: "); Serial.println(dongGoi.loi);
}

bool readRFID() {
  unsigned long startTime = millis();
  while (millis() - startTime < RFID_TIMEOUT) {
    if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
      if (rfid.uid.size <= 4) {
        for (byte i = 0; i < rfid.uid.size; i++) {
          currentUID[i] = rfid.uid.uidByte[i];
        }
        tft.fillScreen(ILI9341_BLACK);
        tft.setCursor(0, 0);
        tft.setTextColor(ILI9341_YELLOW);
        tft.print("UID: ");
        for (byte i = 0; i < rfid.uid.size; i++) {
          tft.print(currentUID[i], HEX);
          tft.print(" ");
        }
        tft.println();
        Serial.print("UID: ");
        for (byte i = 0; i < 4; i++) {
          Serial.print(currentUID[i], HEX);
          Serial.print(" ");
        }
        Serial.println();
      }
      rfid.PICC_HaltA();
      rfid.PCD_StopCrypto1();
      return true;
    }
  }
  return false;
}