/*
   Copyright 2015-2017 James Fong

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

#ifndef PGG_Vec2f_HPP
#define PGG_Vec2f_HPP

#include <ostream>

namespace pgg {

class Vec2 {
public:
    float x;
    float y;

    Vec2();
    Vec2(float x, float y);
    Vec2(const Vec2 &v); // Copy cstr
    ~Vec2();
    
    // Equals
    bool operator==(const Vec2& v);
    bool operator!=(const Vec2& v);
    
    // Scaling
    Vec2 operator*(float s);
    Vec2& operator*=(float s);
    
    // Inverse scaling
    Vec2 operator/(float s);
    Vec2& operator/=(float s);
    
    // Addition
    Vec2 operator+(const Vec2& v);
    Vec2& operator+=(const Vec2& v);
    Vec2 operator+(float s);
    Vec2& operator+=(float s);
    
    // Subtraction
    Vec2 operator-(const Vec2& v);
    Vec2& operator-=(const Vec2& v);
    Vec2 operator-(float s);
    Vec2& operator-=(float s);
    
    // Dot product
    float dot(const Vec2& v2) const;
    float determinant(const Vec2& v2) const;
    
    // Distance to other vector
    float dist(const Vec2& v) const;
    float distSq(const Vec2& v) const;
    
    // Length of vector
    float mag() const;
    float magSq() const;
    
    // Normalize this vector
    void normalize();
    // Normalized vector
    Vec2 normalized() const;
    
    // Make zero
    void zero();
    
    // x == 0 && z == 0
    bool isZero();
};

}

// For debugging
std::ostream& operator<<(std::ostream& os, const pgg::Vec2& v);

#endif // PGG_Vec2f_HPP
