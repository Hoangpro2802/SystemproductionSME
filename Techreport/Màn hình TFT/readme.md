# Hướng dẫn sử dụng màn hình TFT ILI9341 với ESP32

## 1. Giới thiệu
Màn hình **TFT ILI9341 (2.4''/2.8'')** là loại màn hình màu giao tiếp **SPI**.
Độ phân giải: **240 x 320 pixels**.  
Có thể hiển thị chữ, số, menu, hình vẽ và cả hình ảnh BMP.

---

## 2. Kết nối chân (ESP32 → ILI9341)

| ESP32 pin | TFT pin    |
|-----------|------------|
| 23 (MOSI) | SDI (MOSI) |
| 19 (MISO) | SDO (MISO) |
| 18 (SCK)  | SCK        |
| 15        | CS         |
| 2         | DC         |
| 4         | RST        |
| 3V3       | VCC        |
| GND       | GND        |

---

## 3. Thư viện cần cài
Mở Arduino IDE → Library Manager → cài:  
- `Adafruit GFX`  
- `Adafruit ILI9341`

---

## 4. Cách hoạt động
- TFT giao tiếp với ESP32 bằng **SPI**.  
- Khi gửi lệnh vẽ/ghi dữ liệu → hiển thị ngay trên màn hình.  
- Có thể hiển thị:  
  - Văn bản (text, số)  
  - Hình học cơ bản (đường thẳng, hình chữ nhật, hình tròn)  
  - Giao diện menu (chọn công việc, check-in, check-out)  

---

## 5. Ứng dụng trong dự án
- Hiển thị **menu chọn công việc** khi bấm nút PBS.  
- Hiển thị **giờ check-in / check-out** từ WiFi hoặc RTC.  
- Làm giao diện người dùng trực quan thay cho Serial Monitor.  

---

## 6. Code test mẫu
```cpp
tft.fillScreen(ILI9341_BLACK);
tft.setCursor(20, 30);
tft.setTextColor(ILI9341_YELLOW);
tft.setTextSize(2);
tft.println("Xin chao!");

tft.drawRect(10, 70, 200, 40, ILI9341_WHITE);
tft.setCursor(20, 80);
tft.println("1. Check-in");
