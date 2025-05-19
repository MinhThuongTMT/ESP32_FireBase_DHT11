#include <Arduino.h>
#include <WiFiManager.h>
#include <Firebase_ESP_Client.h>
#include <DHT.h>
#include <LiquidCrystal_I2C.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

// Định nghĩa cảm biến DHT
#define DHT_SENSOR_PIN 15
#define DHT_SENSOR_TYPE DHT11
DHT dht_sensor(DHT_SENSOR_PIN, DHT_SENSOR_TYPE);

// Định nghĩa LCD I2C
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Firebase
#define API_KEY "AIzaSyBiYP9n3SArwd1J-yrWoJlVHAEUeB5oCsQ"
#define DATABASE_URL "https://aiotweb-ce263-default-rtdb.asia-southeast1.firebasedatabase.app/"
#define USER_EMAIL "tranminhthuong.tmtek@gmail.com"
#define USER_PASSWORD "Tmt08082003#+##"

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
unsigned long tokenRefreshMillis = 0;
unsigned long lastReconnectAttempt = 0;
int reconnectRetryCount = 0;
const int MAX_RETRIES = 3;
const unsigned long INITIAL_RETRY_INTERVAL = 60000; // 1 minute
const unsigned long MAX_RETRY_INTERVAL = 3600000;   // 1 hour
const unsigned long TOKEN_REFRESH_INTERVAL = 3300000; // 55 minutes

bool initFirebase(bool isRetry = false) {
  if (isRetry) {
    unsigned long backoffTime = min(INITIAL_RETRY_INTERVAL * (1 << reconnectRetryCount), MAX_RETRY_INTERVAL);
    Serial.print("Waiting before retry: ");
    Serial.print(backoffTime / 1000);
    Serial.println(" seconds");
    delay(backoffTime);
  }

  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  config.token_status_callback = tokenStatusCallback;

  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  bool success = Firebase.signUp(&config, &auth, USER_EMAIL, USER_PASSWORD);
  if (!success) {
    Serial.println("Firebase sign up failed");
    return false;
  }

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  
  return Firebase.ready();
}

void setup() {
  Serial.begin(115200);
  dht_sensor.begin();

  // Khởi tạo LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Starting...");
  delay(1000);

  // Khởi tạo WiFiManager
  WiFiManager wm;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WiFi Setup");
  lcd.setCursor(0, 1);
  lcd.print("Connect to AP");

  bool res = wm.autoConnect("ESP32_Setup", "12345678");
  if (!res) {
    Serial.println("Failed to connect WiFi");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("WiFi Failed");
    ESP.restart();
  } else {
    Serial.println("Connected to WiFi");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("WiFi Connected");
    lcd.setCursor(0, 1);
    lcd.print(WiFi.localIP());
    delay(2000);
  }

  // Initial Firebase connection
  reconnectRetryCount = 0;
  while (reconnectRetryCount < MAX_RETRIES) {
    if (initFirebase(reconnectRetryCount > 0)) {
      Serial.println("Firebase connected");
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Firebase OK");
      delay(2000);
      break;
    }
    reconnectRetryCount++;
    if (reconnectRetryCount >= MAX_RETRIES) {
      Serial.println("Failed to connect to Firebase after maximum retries");
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Firebase Error");
      delay(2000);
      ESP.restart();
    }
  }
}

void loop() {
  // Check WiFi connection
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi connection lost");
    ESP.restart();
  }

  // Periodic token refresh
  if (millis() - tokenRefreshMillis > TOKEN_REFRESH_INTERVAL) {
    tokenRefreshMillis = millis();
    Serial.println("Performing periodic token refresh");
    reconnectRetryCount = 0;
    while (reconnectRetryCount < MAX_RETRIES) {
      if (initFirebase(reconnectRetryCount > 0)) {
        Serial.println("Token refreshed successfully");
        break;
      }
      reconnectRetryCount++;
      if (reconnectRetryCount >= MAX_RETRIES) {
        Serial.println("Token refresh failed after maximum retries");
        ESP.restart();
      }
    }
  }

  // Đọc dữ liệu từ DHT11
  float temperature = dht_sensor.readTemperature();
  float humidity = dht_sensor.readHumidity();

  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
    delay(2000);
    return;
  }

  // Hiển thị lên LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(temperature);
  lcd.print(" C");
  lcd.setCursor(0, 1);
  lcd.print("Hum: ");
  lcd.print(humidity);
  lcd.print(" %");

  // Gửi dữ liệu lên Firebase
  if (Firebase.ready() && (millis() - sendDataPrevMillis > 30000)) { // Increased to 30 seconds
    sendDataPrevMillis = millis();
    
    bool success = true;
    
    // Gửi nhiệt độ
    if (!Firebase.RTDB.setFloat(&fbdo, "DHT_11/Temperature", temperature)) {
      Serial.print("Failed to send Temperature: ");
      Serial.println(fbdo.errorReason());
      success = false;
    }

    // Gửi độ ẩm
    if (success && !Firebase.RTDB.setFloat(&fbdo, "DHT_11/Humidity", humidity)) {
      Serial.print("Failed to send Humidity: ");
      Serial.println(fbdo.errorReason());
      success = false;
    }

    // Handle connection issues with exponential backoff
    if (!success && !Firebase.ready()) {
      reconnectRetryCount = 0;
      while (reconnectRetryCount < MAX_RETRIES) {
        if (initFirebase(reconnectRetryCount > 0)) {
          Serial.println("Reconnected successfully");
          break;
        }
        reconnectRetryCount++;
        if (reconnectRetryCount >= MAX_RETRIES) {
          Serial.println("Reconnection failed after maximum retries");
          ESP.restart();
        }
      }
    }
  }

  delay(5000); // Increased main loop delay
}