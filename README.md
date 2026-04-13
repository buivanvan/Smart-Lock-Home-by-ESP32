# 🔐 Smart Door Lock System (ESP32 + RFID + Keypad + MQTT)
Hệ thống đóng/mở khóa cửa tự động bằng ESP32. Các cách thức có thể mở cửa là quét thẻ từ (RFID), nhập mật khẩu, thông qua app điện thoại

## 📌 Sơ đồ kết nối ESP32 với các module

| Thiết bị            | Chân trên module | Chân ESP32 | Ghi chú |
|---------------------|------------------|------------|--------|
|                     | SDA (SS)         | GPIO 16    | SS_PIN |
|                     | SCK              | GPIO 18    | SPI Clock |
|                     | MOSI             | GPIO 23    | SPI MOSI |
| **RFID MFRC522**    | MISO             | GPIO 19    | SPI MISO |
|                     | RST              | GPIO 17    | Reset |
|                     | VCC              | 3.3V       | Nguồn |
|                     | GND              | GND        | Mass |
|---------------------|------------------|------------|--------|
|                     | R1               | GPIO 32    | Hàng |
|                     | R2               | GPIO 33    | Hàng |
|                     | R3               | GPIO 25    | Hàng |
| **Keypad 4x3**      | R4               | GPIO 26    | Hàng |
|                     | C1               | GPIO 27    | Cột |
|                     | C2               | GPIO 14    | Cột |
|                     | C3               | GPIO 12    | Cột |
|---------------------|------------------|------------|--------|
|                     | SDA              | GPIO 21    | I2C Data |
| **LCD I2C 16x2**    | SCL              | GPIO 22    | I2C Clock |
|                     | VCC              | 5V / 3.3V  | Tùy module |
|                     | GND              | GND        | Mass |
|---------------------|------------------|------------|--------|
|                     | IN               | GPIO 15    | Điều khiển relay |
| **Relay Module**    | VCC              | 5V         | Nguồn |
|                     | GND              | GND        | Mass |
|---------------------|------------------|------------|--------|
| **Buzzer**          | Signal           | GPIO 5     | BUZZER |
|                     | VCC              | 3.3V / 5V  | |
|                     | GND              | GND        | |
|---------------------|------------------|------------|--------|
| **LED onboard**     | Anode            | GPIO 2     | ledPin |
|                     | Cathode          | GND        | |
