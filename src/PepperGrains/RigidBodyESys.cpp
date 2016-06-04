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

#include "RigidBodyESys.hpp"

#include <algorithm>

namespace pgg {

RigidBodyESys::RigidBodyMotionListener::RigidBodyMotionListener(const btTransform& initialLoc, RigidBodyComp* const sendTo)
: sendTo(sendTo),
initialLoc(initialLoc) {
}

void RigidBodyESys::RigidBodyMotionListener::getWorldTransform(btTransform& worldTransform) const {
    worldTransform = initialLoc;
}

void RigidBodyESys::RigidBodyMotionListener::setWorldTransform(const btTransform& worldTransform) {
    sendTo->mRotation = worldTransform.getRotation();
    sendTo->mLocation = worldTransform.getOrigin();
    sendTo->mVelocityLinear = sendTo->mRigidBody->getLinearVelocity();
    sendTo->mOnPhysUpdate = true;
}

RigidBodyESys::RigidBodyESys(btDynamicsWorld* dynamicsWorld)
: mDynamicsWorld(dynamicsWorld) {
    mRequiredComponents.push_back(RigidBodyComp::sComponentID);
}

RigidBodyESys::~RigidBodyESys() {
}

void RigidBodyESys::onEntityExists(nres::Entity* entity) {
    RigidBodyComp* comp = (RigidBodyComp*) entity->getComponent(RigidBodyComp::sComponentID);
    
    btTransform trans;
    trans.setIdentity();
    trans.setOrigin(comp->mInitialLoc);
    comp->mMotionState = new RigidBodyMotionListener(trans, comp);
    btVector3 inertia(0, 0, 0);
    comp->mCollisionShape->calculateLocalInertia(comp->mMass, inertia);
    comp->mRigidBody = new btRigidBody(comp->mMass, comp->mMotionState, comp->mCollisionShape, inertia);
    comp->mRigidBody->setUserPointer(entity);
    mDynamicsWorld->addRigidBody(comp->mRigidBody);
    
    mTrackedEntities.push_back(entity);
}
void RigidBodyESys::onEntityDestroyed(nres::Entity* entity) {
    RigidBodyComp* comp = (RigidBodyComp*) entity->getComponent(RigidBodyComp::sComponentID);
    
    mDynamicsWorld->removeRigidBody(comp->mRigidBody);
    delete comp->mMotionState;
    delete comp->mRigidBody;
    
    mTrackedEntities.erase(std::remove(mTrackedEntities.begin(), mTrackedEntities.end(), entity), mTrackedEntities.end());
}
void RigidBodyESys::onEntityBroadcast(nres::Entity* entity, const nres::EntitySignal* data) {
}
const std::vector<nres::ComponentID>& RigidBodyESys::getRequiredComponents() {
    return mRequiredComponents;
}

void RigidBodyESys::onTick() {
    for(std::vector<nres::Entity*>::iterator it = mTrackedEntities.begin(); it != mTrackedEntities.end(); ++ it) {
        nres::Entity* entity = *it;
        RigidBodyComp* rigidBody = (RigidBodyComp*) entity->getComponent(RigidBodyComp::sComponentID);
        
        if(rigidBody->mOnPhysUpdate) {
            // entity->broadcast(new LocationSignal(Vec3f(rigidBody->mLocation)));
            // entity->broadcast(new OrientationSignal(Quate(rigidBody->mRotation)));
            
            rigidBody->mOnPhysUpdate = false;
        }
    }
}

}

