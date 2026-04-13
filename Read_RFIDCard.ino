#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 16
#define RST_PIN 17

MFRC522 mfrc522(SS_PIN, RST_PIN);

void setup() {
  Serial.begin(115200);
  SPI.begin();              // SCK=18, MISO=19, MOSI=23, SS=5
  mfrc522.PCD_Init();

  Serial.println("Quet the RFID...");
}

void loop() {
  // Kiểm tra có thẻ mới
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  // Đọc UID
  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  Serial.print("UID: ");

  // In UID dạng HEX
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    Serial.print(" ");
  }

  Serial.println();

  delay(500); // tránh đọc lặp quá nhanh
}