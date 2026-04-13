#include <SPI.h>
#include <MFRC522.h>
#include <Keypad.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>

#define SS_PIN 16
#define RST_PIN 17
#define BUZZER 5
#define Relay 15

// ===== WiFi =====
const char* ssid = "HONOR500";
const char* password = "123456789";

// ===== MQTT Broker =====
const char* mqtt_server = "de111748f2be41469be6279df3cf2b0a.s1.eu.hivemq.cloud";   // hoặc IP broker của bạn
const int mqtt_port = 8883;
const char* mqtt_user = "buivanvan";
const char* mqtt_pass = "Van.bui6604";
const char* mqtt_topic_sub = "vvan/out";
const char* mqtt_topic_pub = "vvan/in";
// ===== LED =====
const int ledPin = 2;
// ===== Blink WiFi =====
unsigned long previousMillis = 0;
const long blinkInterval = 500;
bool ledState = LOW;

// ===== SSL Client =====
WiFiClientSecure espSecureClient;
PubSubClient client(espSecureClient);

// ===== RFID =============
MFRC522 mfrc522(SS_PIN, RST_PIN);
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ===== RFID =====
byte correctUID1[4] = {0x1A, 0xCC, 0xAC, 0x02};
byte correctUID2[4] = {0x72, 0xE7, 0xA5, 0x41};

// ===== Keypad =====
const byte ROWS = 4;
const byte COLS = 3;

char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};

byte rowPins[ROWS] = {32, 33, 25, 26};
byte colPins[COLS] = {27, 14, 12};
bool ok1, ok2, match;

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

String inputPassword = "";
String correctPassword = "12345";

bool changeMode = false;
// ===== Biến xử lý phím # =====
unsigned long lastHashPress = 0;
unsigned long lastHashDebounce = 0;

const unsigned long doublePressTime = 500;  // 500ms nhấn 2 lần
const unsigned long debounceDelay = 50;     // chống dội 50ms

// ===== Số lần sai # =====
uint8_t num_incorrect =0;

// ===== Relay =======
unsigned long last_open = 0;

// ===== Kết nối WiFi =====
void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Dang ket noi WiFi: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi da ket noi");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

void mo_cua()
{
  digitalWrite(Relay, 1);
  last_open = millis();
}
void mqtt_send(const char* msg)
{
  if (client.connected()) {
    client.publish(mqtt_topic_pub, msg);
    Serial.print("MQTT gui: ");
    Serial.println(msg);
  }
}
// ===== Nhận dữ liệu MQTT =====
void callback(char* topic, byte* payload, unsigned int length) {
  String message = "";

  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  //Serial.print(" | Du lieu: ");
  //Serial.println(message);

  if (message == "1") {
    //Serial.println("Nhan 1");
    mo_cua();
  } 
  else if (message == "0") {
    //Serial.println("Nhan 0");
    digitalWrite(Relay, 0); // đóng cửa
    int last_open = 0;
  } 
}

unsigned long lastMqttRetry = 0;

void reconnect() {
  if (client.connected()) return;

  if (millis() - lastMqttRetry < 5000) return;
  lastMqttRetry = millis();

  Serial.print("Dang ket noi MQTT SSL... ");

  String clientId = "ESP32Client-";
  clientId += String((uint32_t)ESP.getEfuseMac(), HEX);

  if (client.connect(clientId.c_str(), mqtt_user, mqtt_pass)) {
    Serial.println("thanh cong");
    client.subscribe(mqtt_topic_sub);
    Serial.print("Da subscribe topic: ");
    Serial.println(mqtt_topic_sub);
  } else {
    Serial.print("that bai, rc = ");
    Serial.println(client.state());
  }
}

// ===== Chớp LED khi WiFi đã kết nối =====
void blinkLedWhenWifiConnected() {
  if (WiFi.status() == WL_CONNECTED) {
    unsigned long currentMillis = millis();

    if (currentMillis - previousMillis >= blinkInterval) {
      previousMillis = currentMillis;
      ledState = !ledState;
      digitalWrite(ledPin, ledState);
    }
  } else {
    digitalWrite(ledPin, LOW);
  }
}

// ===== Hàm còi =====
void beep_short() {
  digitalWrite(BUZZER, LOW);
  delay(100);
  digitalWrite(BUZZER, HIGH);
}

void beep_long() {
  digitalWrite(BUZZER, LOW);
  delay(1000);
  digitalWrite(BUZZER, HIGH);
}

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  pinMode(Relay, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  digitalWrite(BUZZER, HIGH);

  SPI.begin(18, 19, 23, 16);
  mfrc522.PCD_Init();

  lcd.init();
  lcd.backlight();
  lcd.print("Nhap MK hoac the");

  setup_wifi();

  // Cách nhanh để test TLS
  // Không kiểm tra chứng chỉ server
  espSecureClient.setInsecure();

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {

  blinkLedWhenWifiConnected();

  reconnect();
  client.loop();
  
  if(millis() - last_open > 1500 && digitalRead(Relay)== 1)
  {
    last_open = 0;
    digitalWrite(Relay, 0);
    mqtt_send("1");
  }

  // ====== 1️⃣ KIỂM TRA RFID ======
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {

    match = false;

    if (mfrc522.uid.size == 4) {
      ok1 = true;
      ok2 = true;

      for (byte i = 0; i < 4; i++) {
        if (mfrc522.uid.uidByte[i] != correctUID1[i]) ok1 = false;
        if (mfrc522.uid.uidByte[i] != correctUID2[i]) ok2 = false;
      }

      match = ok1 || ok2;
    }

    lcd.clear();

    if (match) {
      lcd.setCursor(0, 0);
      lcd.print("Mo cua (RFID)");
      lcd.setCursor(0, 1);
      lcd.print("Nguoi mo: ");
      if(ok1) lcd.print("Van");
      else if(ok2) lcd.print("Viet");
      mo_cua();
      mqtt_send("0");
      num_incorrect = 0;
      beep_short();
    } else {
      lcd.print("The sai!");
      digitalWrite(Relay, 0);
      num_incorrect ++;
      if(num_incorrect > 2) beep_long();
      else beep_short();
    }

    delay(500);
    lcd.clear();
    lcd.print("Nhap MK hoac the");

    mfrc522.PICC_HaltA();
  }

  // ====== 2️⃣ KIỂM TRA KEYPAD ======
  char key = keypad.getKey();

   if (key) {

    beep_short();

    // ====== XỬ LÝ PHÍM # ======
    if (key == '#') {

    unsigned long currentTime = millis();

    // ===== CHỐNG DỘI 50ms =====
    if (currentTime - lastHashDebounce < debounceDelay) {
      return;   // bỏ qua nếu bị dội
    }

    lastHashDebounce = currentTime;

    // ===== KIỂM TRA NHẤN 2 LẦN =====
    if (currentTime - lastHashPress <= doublePressTime) {
      changeMode = true;
      inputPassword = "";
      lcd.clear();
      lcd.print("Nhap MK moi:");
    }
    else {
      // Nhấn 1 lần → xóa 1 ký tự
      if (inputPassword.length() > 0) {
        inputPassword.remove(inputPassword.length() - 1);
      }

      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Nhap mat khau:");
      lcd.setCursor(0,1);
      lcd.print(inputPassword);
    }

    lastHashPress = currentTime;
    return;
    }

    // ====== PHÍM * ======
    if (key == '*') {

      lcd.clear();

      if (changeMode) {
        correctPassword = inputPassword;   // lưu mật khẩu mới
        lcd.print("Da doi MK!");
        changeMode = false;
      }
      else {
        if (inputPassword == correctPassword) {
          lcd.print("Nhap dung");
          mo_cua();
          mqtt_send("0");
          num_incorrect = 0;
          beep_short();
        } else {
          lcd.print("Nhap sai");
          digitalWrite(Relay, 0);
          num_incorrect ++;

          if(num_incorrect > 2) beep_long();
        }
      }

      delay(500);

      inputPassword = "";
      lcd.clear();
      lcd.print("Nhap mat khau:");
      return;
    }

    // ====== NHẬP SỐ ======
    inputPassword += key;

    lcd.setCursor(0,1);
    lcd.print("                ");  // xóa dòng
    lcd.setCursor(0,1);
    lcd.print(inputPassword);
  }
}
