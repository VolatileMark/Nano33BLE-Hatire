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


  packet.begin = 0xAAAA;
  packet.end = 0x5555;
  packet.frame = 0;
}

void loop() {

  if (IMU.gyroscopeAvailable()) {
    IMU.readGyroscope(packet.gyro[1], packet.gyro[2], packet.gyro[0]);
  }
  
  Serial.write((uint8_t*) &packet, sizeof(packet));
  
  if (++packet.frame > 999) {
    packet.frame = 0;
  }

}