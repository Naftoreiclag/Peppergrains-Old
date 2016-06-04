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

#ifndef PGG_RIGIDBODYCOMP_HPP
#define PGG_RIGIDBODYCOMP_HPP

#include "btBulletDynamicsCommon.h"

#include "NRES.hpp"

namespace pgg {

class RigidBodyEComp : public nres::Component {
public:
    RigidBodyEComp(btCollisionShape* collisionShape, const float& mass = 1);
    ~RigidBodyEComp();
    
    btVector3 mArgInitialLoc;
    
    btQuaternion mRotation;
    btVector3 mLocation;
    btVector3 mVelocityLinear;
    
    bool mOnPhysUpdate;
    
    const float mMass;
    
    btCollisionShape* mCollisionShape; // For deletion
    btRigidBody* mRigidBody;
    btMotionState* mMotionState;
public:
    static const nres::ComponentID sComponentID;
    const nres::ComponentID& getID() const;

};

}

#endif // PGG_RIGIDBODYCOMP_HPP