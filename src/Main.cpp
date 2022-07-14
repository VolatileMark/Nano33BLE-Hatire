#include <Arduino_LSM9DS1.h>
#include <MadgwickAHRS.h>
#include <Led.h>

struct packet {
  int16_t begin;
  uint16_t frame;
  float gyro[3];
  float accl[3];
  int16_t end;
} __attribute__((packed));
typedef struct packet packet_t;

static packet_t packet;
static Madgwick filter;

#define pYaw packet.gyro[0]
#define pRoll packet.gyro[1]
#define pPitch packet.gyro[2]

void setup() {
  Serial.begin(115200);
  while (!Serial);

  initLeds();

  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU");
    setRgbLedColor(LED_COLOR_RED);
    while (true);
  }
  IMU.setContinuousMode();

  memset(&packet, 0, sizeof(packet_t));
  packet.begin = 0xAAAA;
  packet.end = 0x5555;
  packet.frame = 0;

  filter.begin(IMU.accelerationSampleRate());

  setRgbLedColor(LED_COLOR_GREEN);
}

void loop() {
  float xAcc, yAcc, zAcc;
  float xGyro, yGyro, zGyro;

  if (IMU.accelerationAvailable() && IMU.gyroscopeAvailable()) {
    IMU.readAcceleration(xAcc, yAcc, zAcc);
    IMU.readGyroscope(xGyro, yGyro, zGyro);
    filter.updateIMU(-xGyro, -yGyro, -zGyro, xAcc, yAcc, zAcc);
    pRoll = filter.getRoll();
    pPitch = filter.getPitch();
    pYaw = filter.getYaw() - 180.0f;
  }

  Serial.write((uint8_t*) &packet, sizeof(packet_t));
  if (++packet.frame > 999) {
    packet.frame = 0;
  }
}