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

#include "MathUtil.hpp"

namespace pgg
{

namespace Math {
    
float nearestRightAngle(const float& radians) {
    if(radians == 0.f) {
        return 0.f;
    }
    
    const float full = 6.2831f;
    
    float wrap = fmod(fmod(radians, full) + full, full);
    
    if(wrap < 0.7854f) {
        return 0.0000f;
    } else if(wrap < 2.3562f) {
        return 1.5708f;
    } else if(wrap < 3.9270f) {
        return 3.1416f;
    } else if(wrap < 5.4978f) {
        return 4.7124f;
    } else {
        return 0.0000f;
    }
}
    
float nearestPointOn3DLine(const Vec3& s1, const Vec3& s2, const Vec3& m1, const Vec3& m2) {
    Vec3 mouse = m2 - m1;
    Vec3 street = s2 - s1;
    float mouseMagSq = mouse.magSq();
    float streetMagSq = street.magSq();
    
    Vec3 originDisp = s1 - m1;
    
    float originDotMouse = originDisp.dot(mouse);
    float originDotStreet = originDisp.dot(street);
    float mouseDotStreet = mouse.dot(street);
    
    float divisor = mouseMagSq * streetMagSq - mouseDotStreet * mouseDotStreet;
    if(divisor == 0.f) {
        return std::numeric_limits<float>::infinity();
    } else {
        return (originDotMouse * mouseDotStreet - originDotStreet * mouseMagSq) / divisor;
    }
}
    
float toNearestMultiple(const float& a, const float& b) {
    return std::floor((a / b) + 0.5) * b;
}

float cotangent(const float& radians) {
    return std::cos(radians) / std::sin(radians);
}

glm::quat quaternionLookAt(Vec3 targetDirection, Vec3 initialDirection, Vec3 upDirection) {
    float dotProd = initialDirection.dot(targetDirection);
    // Already facing direction
    if(std::abs(1.f - dotProd) < 0.000001f) {
        return glm::quat();
    }
    // Facing 180 degrees in the wrong direction
    else if(std::abs(-1.f - dotProd) < 0.000001f) {
        return glm::angleAxis(3.1416f, glm::vec3(upDirection));
    }
    return glm::angleAxis(std::acos(dotProd), glm::vec3(initialDirection.cross(targetDirection).normalized()));
}

bool linesIntersect(Vec3 A, Vec3 B, Vec3 C, Vec3 D) {
    
    Vec3 myDisplacement = B - A;
    float myMagnitude = myDisplacement.mag();
    Vec3 myDirection = myDisplacement / myMagnitude;
    
    Vec3 otherDirection = D - C;
    otherDirection.normalize();
    
    // Cosine of the angle between the directions of both edges
    float angleCos = myDirection.dot(otherDirection);
    
    // Check if lines could be parallel enough (both directions are either both nearly the same or are 180 degrees apart)
    if(std::abs(1.f - angleCos) < 0.0001f || std::abs(-1.f - angleCos) < 0.0001f) {
    }
    else {
        // Not at all parallellish
        return false;
    }
    
    // Check if the magnitude of the cross product between these two edges is close to zero
    {
        // This point a point on the other line which is not too close from the start point
        Vec3 otherPoint;
        
        // Make sure this other point is far away enough to get meaningful data from the cross product
        if((C - A).magSq() < 0.0001f) { // Using magSq to avoid division by zero
            otherPoint = D;
        } else {
            otherPoint = C;
        }
        
        float crossProdMagSq = ((otherPoint - A).cross(myDisplacement)).magSq();
        
        if(crossProdMagSq > 0.0001f) {
            return false;
        }
    }
    
    float fracS = (C - A).dot(myDirection) / myMagnitude;
    float fracE = (D - A).dot(myDirection) / myMagnitude;
    
    /* Valid cases:
     * 
     * Caught by the first check:
     *       A---------A
     *    B-----B
     *       A---------A
     *       B----B
     *       A---------A
     *          B---B
     *       A---------A
     *             B---B
     *       A---------A
     *               B-----B
     * Caught by the second check:
     *       A---------A
     *       B---------B
     * 
     * Caught by the third check:
     *       A---------A
     *    B---------------B
     */
     
    bool onEdgeS = abs(fracS) < 0.0001f || abs(1 - fracS) < 0.0001f;
    bool insideS = fracS > 0.f && fracS < 1.f && !onEdgeS;
    bool outsideS = !insideS && !onEdgeS;
    
    bool onEdgeE = abs(fracE) < 0.0001f || abs(1 - fracE) < 0.0001f;
    bool insideE = fracE > 0.f && fracE < 1.f && !onEdgeE;
    bool outsideE = !insideE && !onEdgeE;
    
    // If one of the points are within this edge, then the two connect
    if(insideE || insideS) { return true; }
    
    // If both points line up with this edge exactly, then the two connect
    if(onEdgeS && onEdgeE) { return true; }
    
    // If both points are outside, then two connect only if the points are on opposite sides
    if(outsideS && outsideE && ((fracS < 0.f && fracE > 1.f) || (fracE < 0.f && fracS > 1.f))) { return true; }
    
    return false;
    
}

}

}

