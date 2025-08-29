# Hướng dẫn sử dụng nút nhấn PBS 13mm

## 1. Giới thiệu
Nút nhấn PBS 13mm là loại **nút nhấn nhả** (Momentary Push Button Switch), 
thường dùng trong các mạch điện tử, Arduino, ESP32, v.v.  

Khi nhấn: tiếp điểm đóng lại → mạch được nối.  
Khi thả: tiếp điểm tự động hở ra → mạch ngắt.  

Kích thước phần mũ nhấn khoảng **13mm**, dễ thao tác bằng tay.  

---

## 2. Cấu tạo chân
Nút PBS loại 2 chân (hoặc 4 chân nhưng thực chất là song song 2–2):

- 1 cặp chân nối với nhau khi **nhấn**.
- Khi **không nhấn** → hở mạch (không có dòng đi qua).
- Khi **nhấn** → đóng mạch (dòng đi qua).

Trong mạch Arduino/ESP32 thường đấu như sau:

- **Một chân → GND**
- **Chân còn lại → GPIO (chân số của vi điều khiển)**  
  và bật `INPUT_PULLUP` để dùng điện trở kéo lên nội bộ.

---

## 3. Sơ đồ đấu dây
+3.3V/5V
│
[Internal Pull-up]
│
ESP32 GPIO -----> BTN -----> GND

Khi nút chưa nhấn → GPIO đọc **HIGH**  
Khi nhấn nút → GPIO nối GND → đọc **LOW**

---

## 4. Đặc tính
- Loại: Momentary (Nhấn nhả)
- Kích thước mũ nhấn: 13mm
- Dòng chịu đựng: ~50mA (dùng cho tín hiệu điều khiển, không tải lớn)
- Điện áp tối đa: 12V – 24V (tín hiệu)
- Tuổi thọ: hàng chục nghìn lần nhấn

---

## 5. Ứng dụng
- Làm nút **Menu chọn chức năng**
- Nút **Start / Stop**
- Nút **Reset / Check-in / Check-out**
- Các hệ thống cần **nhập lựa chọn thủ công**

---

## 6. Code mẫu (ESP32/Arduino)

```cpp
#define BTN 13

void setup() {
  Serial.begin(9600);
  pinMode(BTN, INPUT_PULLUP); // dùng điện trở kéo lên nội bộ
}

void loop() {
  if (digitalRead(BTN) == LOW) {
    Serial.println("Nút PBS 13mm được nhấn!");
    delay(200); // chống dội đơn giản
  }
}
