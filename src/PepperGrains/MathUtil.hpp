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

namespace pgg {
namespace Math {

// Closest right angle (in radians) to a given angle (in radians)
float nearestRightAngle(const float& radians);

// Closest point on line A to line B as a kind of approximation of a true intersection (returned as a fraction between A1 and A2)
float nearest3DLineIntersection(const Vec3& A1, const Vec3& A2, const Vec3& B1, const Vec3& B2);

// Rounds A to the nearest whole multiple of B
float nearestMultiple(const float& A, const float& B);

// Cotangent
float cotangent(const float& radians);

// Similar to traditional lookAt functions but in the form of a quaternion transform
glm::quat quaternionLookAt(Vec3 targetDirection, Vec3 initialDirection, Vec3 upDirection);

// True iff line segments A and B are parallel and have points in common
bool lineSegmentsColinear(Vec3 A1, Vec3 A2, Vec3 B1, Vec3 B2);

}
}



#endif // PGG_MATHUTIL_HPP
