# 🔐 Smart Door Lock System (ESP32 + RFID + Keypad + MQTT)
Hệ thống đóng/mở khóa cửa tự động bằng ESP32. Các cách thức có thể mở cửa là quét thẻ từ (RFID), nhập mật khẩu, thông qua ứng dụng điện thoại

## 📌 Sơ đồ kết nối ESP32 với các module

| Thiết bị            | Chân trên module | Chân ESP32 | Ghi chú |
|---------------------|------------------|------------|--------|
| **RFID MFRC522**    | SDA (SS)         | GPIO 16    | SS_PIN |
|                     | SCK              | GPIO 18    | SPI Clock |
|                     | MOSI             | GPIO 23    | SPI MOSI |
|                     | MISO             | GPIO 19    | SPI MISO |
|                     | RST              | GPIO 17    | Reset |
|                     | VCC              | 3.3V       | Nguồn |
|                     | GND              | GND        | Mass |
|---------------------|------------------|------------|--------|
| **Keypad 4x3**      | R1               | GPIO 32    | Hàng |
|                     | R2               | GPIO 33    | Hàng |
|                     | R3               | GPIO 25    | Hàng |
|                     | R4               | GPIO 26    | Hàng |
|                     | C1               | GPIO 27    | Cột |
|                     | C2               | GPIO 14    | Cột |
|                     | C3               | GPIO 12    | Cột |
|---------------------|------------------|------------|--------|
| **LCD I2C 16x2**    | SDA              | GPIO 21    | I2C Data |
|                     | SCL              | GPIO 22    | I2C Clock |
|                     | VCC              | 5V / 3.3V  | Tùy module |
|                     | GND              | GND        | Mass |
|---------------------|------------------|------------|--------|
| **Relay Module**    | IN               | GPIO 15    | Điều khiển relay |
|                     | VCC              | 5V         | Nguồn |
|                     | GND              | GND        | Mass |
|---------------------|------------------|------------|--------|
| **Buzzer**          | Signal           | GPIO 5     | BUZZER |
|                     | VCC              | 3.3V / 5V  | |
|                     | GND              | GND        | |
|---------------------|------------------|------------|--------|
| **LED onboard**     | Anode            | GPIO 2     | ledPin |
|                     | Cathode          | GND        | |


Thay đổi thông tin của Wifi để kết nối với server MQTT ở [hai đoạn mã này:](https://github.com/buivanvan/Smart-Lock-Home-by-ESP32/blob/main/Smart_Door_Lock.ino#L16-L17)
<pre>
<code>16  const char* ssid = "HONOR500";
17  const char* password = "123456789";</code>
</pre>

Thay đổi thông tin của broker MQTT [tại:](https://github.com/buivanvan/Smart-Lock-Home-by-ESP32/blob/main/Smart_Door_Lock.ino#L20-L25)
<pre>
<code>20  const char* mqtt_server = "de111748f2be41469be6279df3cf2b0a.s1.eu.hivemq.cloud";   // hoặc IP broker của bạn
21  const int mqtt_port = 8883;
22  const char* mqtt_user = "buivanvan";
23  const char* mqtt_pass = "************";
24  const char* mqtt_topic_sub = "vvan/out";
25  const char* mqtt_topic_pub = "vvan/in";</code>
</pre>

Nhận broker MQTT miễn phí tại https://www.hivemq.com/
-> Create New Cluster -> Manage Cluster -> Có thể xem được thông tin cụ thể của broker MQTT
<img width="2173" height="702" alt="image" src="https://github.com/user-attachments/assets/d59a827c-3db0-46b9-b089-79a40ca12a9d" />

Về phần app điện thoại, tôi dùng MIT App Inventor để build một ứng dụng điện thoại
Đầu tiên hãy import file .aia của tôi -> Sau đó chỉnh sửa thông tin broker, mật khẩu khi vào ứng dụng -> Sử dụng để mở khóa bằng ứng dụng điện thoại
Để thay đổi mật khẩu để vào ứng dụng, ta chọn tab "Block"
<img width="79" height="56" alt="image" src="https://github.com/user-attachments/assets/116a2490-1bc8-4f21-b572-2008f0f27b23" />
và thay đổi giá trị của biến "password" thành mật khẩu ta mong muốn.
<img width="462" height="64" alt="image" src="https://github.com/user-attachments/assets/75fb6c0f-4386-42da-9104-00913d7f48ad" />

Để thay đổi thông tin broker thì ta cần chuyển sang Screen khác bằng cách chọn dấu mũi tên ở chữ "Screen 1" 
<img width="195" height="129" alt="image" src="https://github.com/user-attachments/assets/3d4aff80-916e-4f15-9de8-0c846258be36" />

-> Chọn tab "Design"
<img width="111" height="58" alt="image" src="https://github.com/user-attachments/assets/8e753e93-74f9-4c2c-b1ad-68570ec9f618" />

Tiếp theo nhấn biểu tượng logo như ảnh
<img width="320" height="126" alt="image" src="https://github.com/user-attachments/assets/f41e62ba-c854-4ed3-ae59-a4ed1ed88928" />

Sau đó bên phải sẽ có một bảng thành phần hiện ra, ta chỉ cần chỉnh sửa những thông tin broker của bản thân sao cho đúng
<img width="312" height="1158" alt="image" src="https://github.com/user-attachments/assets/e4bff688-0b85-4b84-bbf6-3c00ebe50bbd" />

Cuối cùng ta chọn "Build" -> "Android App (.apk)" và tải về điện thoại để cài ứng dụng.
