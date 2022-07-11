#include <Arduino.h>
#include <Arduino_LSM9DS1.h>
#include "led.h"

#define START_BYTE 'S'

struct packet {
  int16_t begin;
  uint16_t frame;
  float gyro[3];
  float accl[3];
  int16_t end;
} __attribute__((packed));
typedef struct packet packet_t;

static packet_t packet;

#define SMOOTH_WEIGHT 0.75f
#define GYRO_WEIGHT 0.10f

static float accx = 0, accy = 0, accz = 0;
static float rotx = 0, roty = 0, rotz = 0;
static float startT;

#define yaw packet.gyro[0]
#define roll packet.gyro[1]
#define pitch packet.gyro[2]

void setup() {
  Serial.begin(115200);
  while (!Serial);

  initLeds();

  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU");
    setRgbLedColor(LED_COLOR_RED);
    while (true);
  }

  memset(&packet, 0, sizeof(packet_t));
  packet.begin = 0xAAAA;
  packet.end = 0x5555;
  packet.frame = 0;

  setRgbLedColor(LED_COLOR_GREEN);

  startT = millis();
}

#define smooth(sx, dx) (((1 - SMOOTH_WEIGHT) * (sx)) + (SMOOTH_WEIGHT * (dx)))
#define fuse(g, am) ((GYRO_WEIGHT * (g)) + ((1- GYRO_WEIGHT) * (am)))

void update() {
  if (!IMU.accelerationAvailable() || !IMU.gyroscopeAvailable()) {
    return;
  }

  float ax, ay, az;
  IMU.readAcceleration(ax, ay, az);
  accx = smooth(accx, ax);
  accy = smooth(accy, ay);
  accz = smooth(accz, az);

  float rx, ry, rz;
  IMU.readGyroscope(rx, ry, rz);
  rotx = smooth(rotx, rx);
  roty = smooth(roty, ry);
  rotz = smooth(rotz, rz);

  float acc_roll = atan2(-accy, sqrt((accx * accx) + (accz * accz))) * RAD_TO_DEG;
  float acc_pitch = atan2(accx, sqrt((accy * accy) + (accz * accz))) * RAD_TO_DEG;

  float deltaT = ((float)(millis() - startT)) / 1000.0f;
  float new_roll = fuse(roll + (roty * deltaT), acc_roll);
  float new_pitch = fuse(pitch + (rotx * deltaT), acc_pitch);
  float new_yaw = yaw + (rotz * deltaT);

  roll = smooth(roll, new_roll);
  pitch = smooth(pitch, new_pitch);
  yaw = smooth(yaw, new_yaw);

  startT = millis();
}

void loop() {
  update();
  Serial.write((uint8_t*) &packet, sizeof(packet_t));
  if (++packet.frame > 999) {
    packet.frame = 0;
  }
}