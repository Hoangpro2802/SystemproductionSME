# 🔌 Kết nối RFID RC522 với STM32+ STM32 kết nối với PI4
## Người làm: Nguyễn Bá Việt Hoàng - B21DCDT100
> Dự án này nhằm mục đích lập trình kết nối STM32 với RFID và PI4.
---

## 📑 Mục Lục

- [Giới thiệu](#giới-thiệu)
- [Danh sách linh kiện](#danh-sách-linh-kiện)
- [Sơ đồ nguyên lý](#sơ-đồ-nguyên-lý)
- [Hướng dẫn lắp ráp](#hướng-dẫn-lắp-ráp)
- [Lập trình firmware](#lập-trình-firmware)
- [Cách sử dụng](#cách-sử-dụng)
- [Ảnh/Video demo](#ảnhvideo-demo)
- [Đóng góp](#đóng-góp)
- [Giấy phép](#giấy-phép)

---

## 👋 Giới Thiệu

STM32F103C8T6 được lập trình để có thể giao tiếp với RFID RC522 cũng như gửi dữ liệu đến PI4 qua UART. Mỗi lần dùng thẻ từ quẹt vào RFID thì UID sẽ được gửi đến STM32 rồi gửi đến PI4. Trên PI4 sẽ kiểm tra UID đó để xác minh hoặc đăng ký.

---

## 📐 Thông Số Kỹ Thuật

| Thành phần     | Thông tin            |
|----------------|----------------------|
| MCU            | STM32F103C8T6        |
| Nguồn vào      | 3V                   |
| Kết nối        | SPI,UART             |

---

## 🧰 Danh Sách Linh Kiện

| Tên linh kiện            | Số lượng | Ghi chú                     |
|--------------------------|----------|-----------------------------|
| STM32F103C8T6            | 1        | Vi điều khiển chính         |
| RFID RC522               | 1        | Truyền dữ liệu              |
| TTL to USB CH340G        | 1        | Đèn báo nguồn               |



---

## 🔧 Sơ Đồ Nguyên Lý và PCB

- 📎 [Schematic (PDF)](docs/Schematic_Lora.pdf)
- 📎 [PCB Layout (Gerber)](docs/Gerber.zip)
- 📎 [File thiết kế (Altium)](docs/fullprojectdesign.zip)

_Hình minh họa sơ đồ nguyên lý hoặc board PCB có thể nhúng ngay tại đây:_

![Schematic](docs/images/Schematic_Lora.jpg)
![PCB](docs/images/PCB.png)


---

## 🔩 Hướng Dẫn Lắp Ráp

1. Bôi thiếc hàn lên tất cả các pad smd.
2. Đặt các linh kiện smd lên. 
3. Đặt ESP32S lên pad hàn.
4. Bôi thiếc hàn lên trên chân của ESP32.
5. Khò hàn tất cả cho đến khi thiếc chảy hết.
6. Hàn tiếp các linh kiện cắm bằng mũi hàn.
7. Kiểm tra bằng mắt, kết hợp đồng hồ đo.
8. Nạp firmware.


---

## 💻 Lập Trình Firmware

- **Ngôn ngữ:** C++ (Arduino)
- 📎 [SourceCode](Sourcecode)
- **Cách nạp:** Sau khi đã compile thành công chương trình, bắt đầu nhấn nụt nạp trên Arduino IDE. Giữ nút boot trên board rồi đợi đến lúc IDE hiện chữ connecting...rồi nhấn và thả nút Reset.
  
## 📜 Cách sử dụng

- Sau khi đã nạp xong firmware, ta sẽ cấp nguồn 5V cho cả 3 node.
- Sau khi cấp nguồn thì nhấn nút EN/Reset trên cả 3 mạch.
- Muốn xem log truyền tin giữa các node thì có thể cấp nguồn bằng UART.
- Kiểm tra trên web Adafruit.io nếu đã nhận được dữ liệu đẩy lên thì mạch đã hoạt động đúng.

## 💿 Kiểm thử

![WEB](docs/images/progress.png)
![Mạch](docs/images/lora.jpg)



## 🎞️ Ảnh/Video demo
https://private-user-images.githubusercontent.com/156501341/435402061-04e07901-b8c4-47a6-8766-bfb23d1321f1.mp4?jwt=eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJnaXRodWIuY29tIiwiYXVkIjoicmF3LmdpdGh1YnVzZXJjb250ZW50LmNvbSIsImtleSI6ImtleTUiLCJleHAiOjE3NDUwNzUwODMsIm5iZiI6MTc0NTA3NDc4MywicGF0aCI6Ii8xNTY1MDEzNDEvNDM1NDAyMDYxLTA0ZTA3OTAxLWI4YzQtNDdhNi04NzY2LWJmYjIzZDEzMjFmMS5tcDQ_WC1BbXotQWxnb3JpdGhtPUFXUzQtSE1BQy1TSEEyNTYmWC1BbXotQ3JlZGVudGlhbD1BS0lBVkNPRFlMU0E1M1BRSzRaQSUyRjIwMjUwNDE5JTJGdXMtZWFzdC0xJTJGczMlMkZhd3M0X3JlcXVlc3QmWC1BbXotRGF0ZT0yMDI1MDQxOVQxNDU5NDNaJlgtQW16LUV4cGlyZXM9MzAwJlgtQW16LVNpZ25hdHVyZT1jMDQwNTRlZjgzOWYzNGIxODNiZDE5ZWE0YjE0Y2U0M2MxOGZmYzcxOTgzMmQxMmI2ZWI0NDliZGNiMWQzZDhmJlgtQW16LVNpZ25lZEhlYWRlcnM9aG9zdCJ9.2w96RrguRFl_zu4LSO7ULRKF_O6pDouDRltqa7MFOOQ

Xem thêm các video demo khác tại:
- 📎 [DEMOVIDEO](docs/images)


## 👐 Đóng góp

Dự án được thực hiện với mục đích học tập và nghiên cứu. Mọi ý kiến đóng góp, đề xuất hoặc phản hồi đều được hoan nghênh. Nếu bạn muốn đóng góp, vui lòng tạo một Pull Request hoặc Issue để thảo luận thêm.


## ©️ Giấy phép

Dự án này được phát hành dưới giấy phép MIT License. Bạn có thể tự do sử dụng, chỉnh sửa và phân phối lại với điều kiện giữ nguyên thông tin bản quyền.




  

