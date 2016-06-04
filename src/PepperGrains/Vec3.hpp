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

#ifndef PGG_VEC3F_HPP
#define PGG_VEC3F_HPP

#include <ostream>

#include "btBulletDynamicsCommon.h"
#include "OgreVector3.h"

namespace pgg {

class Vec3f {
public:
    float x;
    float y;
    float z;

    Vec3f();
    Vec3f(float x, float y, float z);
    ~Vec3f();
    
    // Copy constructor
    Vec3f(const Vec3f &v); 
    
    // Implicit conversions
    operator Ogre::Vector3() const;
    Vec3f(const Ogre::Vector3& v);
    operator btVector3() const;
    Vec3f(const btVector3& v);
    
    // Equals
    bool operator==(const Vec3f& v) const;
    bool operator!=(const Vec3f& v) const;
    
    // Scaling
    Vec3f operator*(float s) const;
    Vec3f& operator*=(float s);
    
    // Inverse scaling
    Vec3f operator/(float s) const;
    Vec3f& operator/=(float s);
    
    // Addition
    Vec3f operator+(const Vec3f& v) const;
    Vec3f& operator+=(const Vec3f& v);
    
    // Subtraction
    Vec3f operator-(const Vec3f& v) const;
    Vec3f& operator-=(const Vec3f& v);
    
    // Dot product
    float dot(const Vec3f& v2) const;
    
    // Cross product
    Vec3f cross(const Vec3f& v2) const;
    
    // Distance to other vector
    float dist(const Vec3f& v) const;
    float distSq(const Vec3f& v) const;
    
    // Length of vector
    float mag() const;
    float magSq() const;
    
    // Normalize this vector
    void normalize();
    // Normalized vector
    Vec3f normalized() const;
    
    // Make zero
    void zero();
    
    // x == 0 && y == 0 && z == 0
    bool isZero() const;
    
    // isnan(x) || isnan(y) || isnan(z)
    bool isNan() const;
};

}

// For debugging
std::ostream& operator<<(std::ostream& os, const pgg::Vec3f& v);

#endif // PGG_VEC3F_HPP
