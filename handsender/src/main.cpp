#include <Wire.h>
#include <MPU6050_light.h>
#include <esp_now.h>
#include <WiFi.h>

// MPU6050 Setup
MPU6050 mpu(Wire);
float accX, accY, accZ;

// ESP-NOW
uint8_t receiverMac[] = {0x84, 0x0D, 0x8E, 0xAB, 0xD4, 0xC2}; // Replace with Receiver MAC Address
String axisData = "";

// ESP-NOW Send Callback
void sendCallback(const uint8_t *macAddr, esp_now_send_status_t status) {
  Serial.print("Send Status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
}

void setup() {
  Serial.begin(115200);

  // Initialize MPU6050
  Wire.begin(21, 22); // SDA=D21, SCL=D22
  byte status = mpu.begin();
  if (status != 0) {
    Serial.println("MPU6050 Initialization Failed!");
    while (1);
  }
  Serial.println("MPU6050 Initialized!");

  mpu.calcOffsets(); // Calculate offsets for calibration
  Serial.println("MPU6050 Offsets Calculated!");

  // Set up Wi-Fi in Station mode
  WiFi.mode(WIFI_STA);

  // Initialize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW Init Failed!");
    ESP.restart();
  }
  esp_now_register_send_cb(sendCallback);

  // Add peer (receiver)
  esp_now_peer_info_t peerInfo;
  memcpy(peerInfo.peer_addr, receiverMac, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer!");
    ESP.restart();
  }
}

void loop() {
  // Update MPU6050 data
  mpu.update();

  // Read accelerometer data
  accX = mpu.getAccX();
  accY = mpu.getAccY();
  accZ = mpu.getAccZ();

  // Prepare the axis data as a string to send
  axisData = "X: " + String(accX) + ", Y: " + String(accY) + ", Z: " + String(accZ);

  // Send axis data to the receiver (ESP8266)
  esp_now_send(receiverMac, (uint8_t *)axisData.c_str(), axisData.length() + 1);
  Serial.println("Sent Data: " + axisData);

  delay(100); // Small delay for stability
}