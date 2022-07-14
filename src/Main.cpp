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

  Vector3f g;
  IMU.readGyroscope(g.x, g.y, g.z);

  Vector3f m;
  IMU.readMagneticField(m.x, m.y, m.z);
  m.x = -m.x;
  
  // https://www.mathworks.com/videos/sensor-fusion-part-2-fusing-a-mag-accel-and-gyro-to-estimate-orientation-1569411056638.html
  Vector3f n, e, d;
  d = -a.norm();
  e = d.cross(m.norm());
  n = e.cross(d);

  // https://www.vectornav.com/resources/inertial-navigation-primer/math-fundamentals/math-attitudetran
  float y1 = atan(e.x / n.x) * RAD_TO_DEG;
  float p1 = atan2(a.x, sqrt(a.y * a.y + a.z * a.z)) * RAD_TO_DEG;
  float r1 = atan2(-a.y, sqrt(a.x * a.x + a.z * a.z)) * RAD_TO_DEG;

  float newT = millis();
  float deltaT = ((float)(newT - startT)) / 1000.0f;
  startT = newT;

  float y2 = yaw + (g.z * deltaT);
  float p2 = pitch + (g.y * deltaT);
  float r2 = roll + (g.x * deltaT);

  float y = fuse(-y1, y2, 0.05f);
  float p = fuse(p1, p2, 0.10f);
  float r = fuse(r1, r2, 0.10f);

  yaw = fuse(yaw, y, 0.10f);
  pitch = fuse(pitch, p, 0.10f);
  roll = fuse(roll, r, 0.10f);
}

void loop() {
  update();
  Serial.write((uint8_t*) &packet, sizeof(packet_t));
  
  if (++packet.frame > 999) {
    packet.frame = 0;
  }
}