#include <ESP8266WiFi.h>
#include <espnow.h>

// Motor driver pins
const int motor1Forward = D1; // Motor 1 forward
const int motor1Backward = D2; // Motor 1 backward
const int motor2Forward = D3; // Motor 2 forward
const int motor2Backward = D4; // Motor 2 backward

// Command buffer to hold received data
char receivedData[50];

// Function to control motors based on gesture
void controlCar(float accX, float accY) {
  if (accX > 0.5) {
    // Move forward
    digitalWrite(motor1Forward, HIGH);
    digitalWrite(motor1Backward, LOW);
    digitalWrite(motor2Forward, HIGH);
    digitalWrite(motor2Backward, LOW);
    Serial.println("Car moving forward");
  } else if (accX < -0.5) {
    // Move backward
    digitalWrite(motor1Forward, LOW);
    digitalWrite(motor1Backward, HIGH);
    digitalWrite(motor2Forward, LOW);
    digitalWrite(motor2Backward, HIGH);
    Serial.println("Car moving backward");
  } else if (accY > 0.5) {
    // Turn right
    digitalWrite(motor1Forward, HIGH);
    digitalWrite(motor1Backward, LOW);
    digitalWrite(motor2Forward, LOW);
    digitalWrite(motor2Backward, LOW);
    Serial.println("Car turning right");
  } else if (accY < -0.5) {
    // Turn left
    digitalWrite(motor1Forward, LOW);
    digitalWrite(motor1Backward, LOW);
    digitalWrite(motor2Forward, HIGH);
    digitalWrite(motor2Backward, LOW);
    Serial.println("Car turning left");
  } else {
    // Stop
    digitalWrite(motor1Forward, LOW);
    digitalWrite(motor1Backward, LOW);
    digitalWrite(motor2Forward, LOW);
    digitalWrite(motor2Backward, LOW);
    Serial.println("Car stopped");
  }
}

// Function to handle received data
void onDataRecv(uint8_t *macAddr, uint8_t *data, uint8_t len) {
  memcpy(receivedData, data, len);
  receivedData[len] = '\0';

  // Print received data
  Serial.print("Received Data: ");
  Serial.println(receivedData);

  // Parse accelerometer values
  String axisData = String(receivedData);
  int xStart = axisData.indexOf("X:") + 2;
  int yStart = axisData.indexOf("Y:") + 2;
  int zStart = axisData.indexOf("Z:") + 2;

  int xEnd = axisData.indexOf(",", xStart);
  int yEnd = axisData.indexOf(",", yStart);
  int zEnd = axisData.indexOf("\0", zStart);

  float accX = axisData.substring(xStart, xEnd).toFloat();
  float accY = axisData.substring(yStart, yEnd).toFloat();
  float accZ = axisData.substring(zStart, zEnd).toFloat();

  // Control car based on parsed values
  controlCar(accX, accY);
}

void setup() {
  Serial.begin(115200);

  // Initialize motor pins
  pinMode(motor1Forward, OUTPUT);
  pinMode(motor1Backward, OUTPUT);
  pinMode(motor2Forward, OUTPUT);
  pinMode(motor2Backward, OUTPUT);

  WiFi.mode(WIFI_STA);

  if (esp_now_init() != 0) {
    Serial.println("ESP-NOW Initialization Failed");
    ESP.restart();
  }

  esp_now_register_recv_cb(onDataRecv);
  Serial.println("ESP-NOW Ready to Receive Data");
}

void loop() {
  // All actions are handled in onDataRecv
}