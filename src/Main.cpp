#include <Arduino_LSM9DS1.h>
#include <Led.h>
#include <Vector.h>

struct packet {
  int16_t begin;
  uint16_t frame;
  float gyro[3];
  float accl[3];
  int16_t end;
} __attribute__((packed));
typedef struct packet packet_t;

static packet_t packet;

static unsigned long startT;

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
  IMU.setContinuousMode();

  memset(&packet, 0, sizeof(packet_t));
  packet.begin = 0xAAAA;
  packet.end = 0x5555;
  packet.frame = 0;

  setRgbLedColor(LED_COLOR_GREEN);

  startT = millis();
}

#define fuse(a, b, w) (((1 - w) * (b)) + (w * (a)))

void update() {
  if (!IMU.accelerationAvailable() || !IMU.gyroscopeAvailable() || !IMU.magneticFieldAvailable()) {
    return;
  }

  Vector3f a;
  IMU.readAcceleration(a.x, a.y, a.z);

  Vector3f m;
  IMU.readMagneticField(m.x, m.y, m.z);

  Vector3f g;
  IMU.readGyroscope(g.x, g.y, g.z);

  Vector3f n, e, d;
  d = -a.norm();
  e = d.cross(m.norm());
  n = e.cross(d);

  float dcm[3][3] = {
    { n.x, e.x, d.x },
    { n.y, e.y, d.y },
    { n.z, e.z, d.z }
  };
  
  float y1 = atan(dcm[0][1] / dcm[0][0]) * RAD_TO_DEG;
  float p1 = asin(dcm[0][2]) * RAD_TO_DEG;
  float r1 = atan(dcm[1][2] / dcm[2][2]) * RAD_TO_DEG;

  float deltaT = ((float)(millis() - startT)) / 1000.0f;
  float y2 = yaw + (g.z * deltaT);
  float p2 = pitch + (g.y * deltaT);
  float r2 = roll + (g.x * deltaT);

  float y = fuse(-y1, y2, 0.70f);
  float p = fuse(-p1, p2, 0.70f);
  float r = fuse(-r1, r2, 0.70f);

  yaw = fuse(yaw, y, 0.10f);
  pitch = fuse(pitch, p, 0.10f);
  roll = fuse(roll, r, 0.10f);

  startT = millis();
}

void loop() {
  update();
  Serial.write((uint8_t*) &packet, sizeof(packet_t));
  
  if (++packet.frame > 999) {
    packet.frame = 0;
  }
}