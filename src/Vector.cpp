#include <Vector.h>
#include <Arduino.h>

Vector3f::Vector3f() : Vector3f(.0f, .0f, .0f) {}
Vector3f::Vector3f(const Vector3f& vec) : Vector3f(vec.x, vec.y, vec.z) {};
Vector3f::Vector3f(float ix, float iy, float iz) : x(ix), y(iy), z(iz) {}

Vector3f& Vector3f::operator=(const Vector3f& vec) {
  x = vec.x;
  y = vec.y;
  z = vec.z;
  return *this;
}

Vector3f Vector3f::operator-() const {
  Vector3f v(-x, -y, -z);
  return v;
}

Vector3f Vector3f::operator+(const Vector3f& vec) const {
  Vector3f v(x + vec.x, y + vec.y, z + vec.z);
  return v;
}

Vector3f Vector3f::operator-(const Vector3f& vec) const {
  return (*this + (-vec));
}

Vector3f Vector3f::cross(const Vector3f& vec) const {
  Vector3f v(
    (y * vec.z) - (z * vec.y), 
    (z * vec.x) - (x * vec.z), 
    (x * vec.y) - (y * vec.x)
  );
  return v;
}


float Vector3f::dot(const Vector3f& vec) const {
  return ((x * vec.x) + (y * vec.y) + (z * vec.z));
}

float Vector3f::len() const {
  return sqrt((x * x) + (y * y) + (z * z));
}

float Vector3f::cos(const Vector3f& vec) const {
  return (this->dot(vec) / (this->len() * vec.len()));
}

float Vector3f::sin(const Vector3f& vec) const {
  return ((this->cross(vec)).len() / (this->len() * vec.len()));
}

float Vector3f::angle(const Vector3f& vec) const {
  return acos(this->cos(vec));
}

void Vector3f::print() const {
  Serial.print("[ ");
  Serial.print(x);
  Serial.print(", ");
  Serial.print(y);
  Serial.print(", ");
  Serial.print(z);
  Serial.println(" ]");
}

Vector3f Vector3f::norm() const {
  float l = this->len();
  Vector3f v(x / l, y / l, z / l);
  return v;
}