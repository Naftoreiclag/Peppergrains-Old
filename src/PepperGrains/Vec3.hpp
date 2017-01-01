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

#ifndef PGG_VEC3F_HPP
#define PGG_VEC3F_HPP

#include <ostream>

#include "btBulletDynamicsCommon.h"

#include "OpenGLStuff.hpp"

namespace pgg {

class Vec3 {
public:
    float x;
    float y;
    float z;

    Vec3();
    Vec3(float x, float y, float z);
    Vec3(float xyz);
    ~Vec3();
    
    // Copy constructor
    Vec3(const Vec3& v); 
    
    // Implicit conversions
    operator btVector3() const;
    Vec3(const btVector3& v);
    operator glm::vec3() const;
    Vec3(const glm::vec3& v);
    Vec3(const glm::vec4& v);
    
    // Comparison
    bool operator==(const Vec3& v) const;
    bool operator!=(const Vec3& v) const;
    
    // (Default assignment operator is used)
    
    // Scaling
    Vec3 operator*(float s) const;
    Vec3& operator*=(float s);
    
    // Inverse scaling
    Vec3 operator/(float s) const;
    Vec3& operator/=(float s);
    
    // Addition
    Vec3 operator+(const Vec3& v) const;
    Vec3& operator+=(const Vec3& v);
    
    // Subtraction
    Vec3 operator-(const Vec3& v) const;
    Vec3& operator-=(const Vec3& v);
    
    // Dot product
    float dot(const Vec3& v2) const;
    
    // Cross product
    Vec3 cross(const Vec3& v2) const;
    
    // Distance to other vector
    float dist(const Vec3& v) const;
    float distSq(const Vec3& v) const;
    
    // Length of vector
    float mag() const;
    float magSq() const;
    
    // Normalize this vector
    void normalize();
    // Normalized vector
    Vec3 normalized() const;
    
    // Make zero
    void zero();
    
    // x == 0 && y == 0 && z == 0
    bool isZero() const;
    
    // isnan(x) || isnan(y) || isnan(z)
    bool isNan() const;
};

}

// For debugging
std::ostream& operator<<(std::ostream& os, const pgg::Vec3& v);

#endif // PGG_VEC3F_HPP
