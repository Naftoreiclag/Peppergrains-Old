/*
   Copyright 2016-2017 James Fong

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

#ifndef PGG_RIGIDBODYESYS_HPP
#define PGG_RIGIDBODYESYS_HPP

#include "btBulletDynamicsCommon.h"

#include "NRES.hpp"

#include "RigidBodyEComp.hpp"

namespace pgg {

class RigidBodyESys : public nres::System {
public:
    class RigidBodyMotionListener : public btMotionState {
    protected:
        btTransform initialLoc;
        RigidBodyEComp* const sendTo;
    public:
        RigidBodyMotionListener(const btTransform& initialLoc, RigidBodyEComp* const sendTo);
        void getWorldTransform(btTransform& worldTransform) const;
        void setWorldTransform(const btTransform& worldTransform);
    };
public:
    RigidBodyESys(btDynamicsWorld* dynamicsWorld);
    ~RigidBodyESys();
private:
    std::vector<nres::ComponentID> mRequiredComponents;
    std::vector<nres::Entity*> mTrackedEntities;
    
    btDynamicsWorld* mDynamicsWorld;
    
public:
    void onEntityExists(nres::Entity* entity);
    void onEntityDestroyed(nres::Entity* entity);
    void onEntityBroadcast(nres::Entity* entity, const nres::EntitySignal* data);
    
    const std::vector<nres::ComponentID>& getRequiredComponents();
    
    void onTick();
};

}

#endif // PGG_RIGIDBODYESYS_HPP
