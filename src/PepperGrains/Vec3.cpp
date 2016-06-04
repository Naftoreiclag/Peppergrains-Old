/*
   Copyright 2015-2016 James Fong

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.

*/

#include "Vec3.hpp"

#include <cmath>
#include <math.h>

namespace pgg {

Vec3::Vec3()
: x(0.f)
, y(0.f)
, z(0.f) {
}

Vec3::Vec3(float x, float y, float z)
: x(x)
, y(y)
, z(z) {
}

Vec3::Vec3(const Vec3 &v)
: x(v.x)
, y(v.y)
, z(v.z) {
}

Vec3::~Vec3() {
}

// Implicit conversions
Vec3::operator btVector3() const {
    return btVector3(x, y, z);
}
Vec3::Vec3(const btVector3& v)
: x(v.getX())
, y(v.getY())
, z(v.getZ()) {
}
Vec3::operator glm::vec3() const {
    return glm::vec3(x, y, z);
}
Vec3::Vec3(const glm::vec3& v)
: x(v.x)
, y(v.y)
, z(v.z) {
}

// Equals
bool Vec3::operator==(const Vec3& v1) const {
    return v1.x == x && v1.y == y && v1.z == z;
}
bool Vec3::operator!=(const Vec3& v1) const {
    return !operator==(v1);
}

// Scaling
Vec3 Vec3::operator*(float s) const {
    return Vec3(x * s, y * s, z * s);
}
Vec3& Vec3::operator*=(float s) {
    x *= s;
    y *= s;
    z *= s;
    return *this;
}

// Inverse scaling
Vec3 Vec3::operator/(float s) const {
    return Vec3(x / s, y / s, z / s);
}
Vec3& Vec3::operator/=(float s) {
    x /= s;
    y /= s;
    z /= s;
    return *this;
}

// Addition
Vec3 Vec3::operator+(const Vec3& v) const {
    return Vec3(x + v.x, y + v.y, z + v.z);
}
Vec3& Vec3::operator+=(const Vec3& v) {
    x += v.x;
    y += v.y;
    z += v.z;
    return *this;
}

// Subtraction
Vec3 Vec3::operator-(const Vec3& v) const {
    return Vec3(x - v.x, y - v.y, z - v.z);
}
Vec3& Vec3::operator-=(const Vec3& v) {
    x -= v.x;
    y -= v.y;
    z -= v.z;
    return *this;
}

// Dot product
float Vec3::dot(const Vec3& v2) const {
    return (x * v2.x) + (y * v2.y) + (z * v2.z);
}

// Cross product
Vec3 Vec3::cross(const Vec3& v2) const {
    return Vec3((y * v2.z) - (v2.y * z), (z * v2.x) - (v2.z * x), (x * v2.y) - (v2.x * y));
}

// Distance to other vector
float Vec3::distSq(const Vec3& v) const {
    float dx = v.x - x;
    float dy = v.y - y;
    float dz = v.z - z;
    return (dx * dx) + (dy * dy) + (dz * dz);
}
float Vec3::dist(const Vec3& v) const {
    return std::sqrt(distSq(v));
}

// Length of vector
float Vec3::magSq() const {
    return (x * x) + (y * y) + (z * z);
}
float Vec3::mag() const {
    return std::sqrt(magSq());
}

// Normalize this vector
void Vec3::normalize() {
    float magnitude = mag();
    x /= magnitude;
    y /= magnitude;
    z /= magnitude;
}
// Normalized vector
Vec3 Vec3::normalized() const {
    float magnitude = mag();
    return Vec3(x / magnitude, y / magnitude, z / magnitude);
}

void Vec3::zero() {
    x = 0.f;
    y = 0.f;
    z = 0.f;
}

bool Vec3::isZero() const {
    return x == 0.f && y == 0.f && z == 0.f;
}
// isnan(x) || isnan(y) || isnan(z)
bool Vec3::isNan() const {
    return std::isnan(x) || std::isnan(y) || std::isnan(z);
}

}
    
// For debugging
std::ostream& operator<<(std::ostream& os, const pgg::Vec3& v) {
    return os << "Vec3[" << v.x << ", " << v.y << ", " << v.z << "]";
}
