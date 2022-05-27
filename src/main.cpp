#include <Arduino.h>
#include <Arduino_LSM9DS1.h>
#include "led.h"

struct packet {
  int16_t begin;
  uint16_t frame;
  float gyro[3];
  float accl[3];
  int16_t end;
};
typedef struct packet packet_t;

static packet_t packet;
static unsigned long start;

void setup() {
  Serial.begin(115200);
  while (!Serial);

  initLeds();

  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU");
    setRgbLedColor(LED_COLOR_RED);
    while (true);
  }
  setRgbLedColor(LED_COLOR_GREEN);


  memset(&packet, 0, sizeof(packet_t));
  packet.begin = 0xAAAA;
  packet.end = 0x5555;
  packet.frame = 0;

  start = millis();
}

void loop() {
  float delta;

  if (IMU.accelerationAvailable()) {
    float x, y, z;
    IMU.readAcceleration(x, y, z);
    packet.gyro[1] = (atan(y / sqrt((x * x) + (z * z))) * 180.0f) / PI;
    packet.gyro[2] = (atan(-x / sqrt((y * y) + (z * z))) * 180.0f) / PI;
  }
  if (IMU.gyroscopeAvailable()) {
    float pitch, roll, yaw;
    delta = ((float)(millis() - start)) / 1000.0f;
    IMU.readGyroscope(pitch, roll, yaw);
    if (abs(yaw) > 1.0f)
      packet.gyro[0] += yaw * delta;
    start = millis();
  }

  Serial.write((uint8_t*) &packet, 30);

  if (++packet.frame > 999) {
    packet.frame = 0;
  }
}