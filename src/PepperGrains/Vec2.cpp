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

#include "Vec2.hpp"

#include <cmath>

namespace pgg {

Vec2::Vec2()
: x(0.f)
, z(0.f) {
}

Vec2::Vec2(float x, float z)
: x(x)
, z(z) {
}

Vec2::Vec2(const Vec2 &v)
: x(v.x)
, z(v.z) {
}

Vec2::~Vec2() {
}

// Equals
bool Vec2::operator==(const Vec2& v1) {
    return v1.x == x && v1.z == z;
}
bool Vec2::operator!=(const Vec2& v1) {
    return !operator==(v1);
}

// Scaling
Vec2 Vec2::operator*(float s) {
    return Vec2(x * s, z * s);
}
Vec2& Vec2::operator*=(float s) {
    x *= s;
    z *= s;
    return *this;
}

// Inverse scaling
Vec2 Vec2::operator/(float s) {
    return Vec2(x / s, z / s);
}
Vec2& Vec2::operator/=(float s) {
    x /= s;
    z /= s;
    return *this;
}

// Addition
Vec2 Vec2::operator+(const Vec2& v) {
    return Vec2(x + v.x, z + v.z);
}
Vec2& Vec2::operator+=(const Vec2& v) {
    x *= v.x;
    z *= v.z;
    return *this;
}

// Subtraction
Vec2 Vec2::operator-(const Vec2& v) {
    return Vec2(x - v.x, z - v.z);
}
Vec2& Vec2::operator-=(const Vec2& v) {
    x *= v.x;
    z *= v.z;
    return *this;
}

// Distance to other vector
float Vec2::distSq(const Vec2& v) {
    float dx = v.x - x;
    float dz = v.z - z;
    return (dx * dx) + (dz * dz);
}
float Vec2::dist(const Vec2& v) {
    return std::sqrt(distSq(v));
}

// Length of vector
float Vec2::magSq() {
    return (x * x) + (z * z);
}
float Vec2::mag() {
    return std::sqrt(magSq());
}

void Vec2::zero() {
    x = 0.f;
    z = 0.f;
}

bool Vec2::isZero() {
    return x == 0.f && z == 0.f;
}

}
    
// For debugging
std::ostream& operator<<(std::ostream& os, const pgg::Vec2& v) {
    return os << "Vec2[" << v.x << ", " << v.z << "]";
}
