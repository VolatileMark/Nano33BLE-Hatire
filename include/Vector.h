#pragma once

class Vector3f {

public:
  float x, y, z;
  Vector3f();
  Vector3f(const Vector3f& vec);
  Vector3f(float x, float y, float z);
  Vector3f& operator=(const Vector3f& vec);
  Vector3f operator-() const;
  Vector3f operator+(const Vector3f& vec) const;
  Vector3f operator-(const Vector3f& vec) const;
  Vector3f cross(const Vector3f& vec) const;
  Vector3f norm() const;
  float dot(const Vector3f& vec) const;
  float len() const;
  float cos(const Vector3f& vec) const;
  float sin(const Vector3f& vec) const;
  float angle(const Vector3f& vec) const;
  void print() const;
};