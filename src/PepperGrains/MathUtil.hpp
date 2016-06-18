/*
   Copyright 2016 James Fong
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

#ifndef PGG_MATHUTIL_HPP
#define PGG_MATHUTIL_HPP

#include "OpenGLStuff.hpp"
#include "Vec3.hpp"
#include "Quate.hpp"

namespace pgg
{

namespace Math {
    
float nearestRightAngle(const float& radians);
    
float nearestPointOn3DLine(const Vec3& s1, const Vec3& s2, const Vec3& m1, const Vec3& m2);
    
float toNearestMultiple(const float& a, const float& b);

float cotangent(const float& radians);

glm::quat quaternionLookAt(Vec3 targetDirection, Vec3 initialDirection, Vec3 upDirection);

bool linesIntersect(Vec3 A, Vec3 B, Vec3 C, Vec3 D);

}

}



#endif // PGG_MATHUTIL_HPP
