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
| RFID RC522               | 1        | Tương tác với thẻ từ        |
| TTL to USB CH340G        | 1        | Chuyển đổi dữ liệu          |
| PI4                      | 1        | Đích                        |



---

## 🔧 Sơ Đồ Nguyên Lý


[!Schematic](Resources/schematic.png)


---

## 🔩 Hướng Dẫn Lắp Ráp

- 📎 [Guild](Guild.docs)


---

## 💻 Lập Trình Firmware

- **Ngôn ngữ:** C++ (Arduino)
- 📎 [SourceCode](maincode.c)
- 📎 [Sourceproject](Resources/rfidsource.rar)
  
## 📜 Cách sử dụng

- 📎 [Guild](Guildư.docs)


## 👐 Đóng góp

Dự án được thực hiện với mục đích học tập và nghiên cứu. Mọi ý kiến đóng góp, đề xuất hoặc phản hồi đều được hoan nghênh. Nếu bạn muốn đóng góp, vui lòng tạo một Pull Request hoặc Issue để thảo luận thêm.


## ©️ Giấy phép

Dự án này được phát hành dưới giấy phép MIT License. Bạn có thể tự do sử dụng, chỉnh sửa và phân phối lại với điều kiện giữ nguyên thông tin bản quyền.




  

