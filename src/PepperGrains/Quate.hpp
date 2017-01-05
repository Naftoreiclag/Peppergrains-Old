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

#ifndef PGG_QUATE_HPP
#define PGG_QUATE_HPP

#include "btBulletDynamicsCommon.h"

#include "GraphicsApiStuff.hpp"

// Nothing but a way to easily convert between the many quaternion types

namespace pgg {

class Quate {
public:
    float x;
    float y;
    float z;
    float w;

    Quate();
    Quate(float w, float x, float y, float z);
    ~Quate();
    
    // Copy-constructor
    Quate(const Quate& q);
    
    // Implicit conversions
    operator glm::quat() const;
    Quate(const glm::quat& q);
    operator btQuaternion() const;
    Quate(const btQuaternion& q);
};

}

#endif // PGG_QUATE_HPP
