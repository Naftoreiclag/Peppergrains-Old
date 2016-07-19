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

#include "PhysicsLocationUpdateESignal.hpp"
#include "PhysicsOrientationUpdateESignal.hpp"

namespace pgg {

RigidBodyESys::RigidBodyMotionListener::RigidBodyMotionListener(const btTransform& initialLoc, RigidBodyEComp* const sendTo)
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
    mRequiredComponents.push_back(RigidBodyEComp::sComponentID);
}

RigidBodyESys::~RigidBodyESys() {
}

void RigidBodyESys::onEntityExists(nres::Entity* entity) {
    RigidBodyEComp* comp = (RigidBodyEComp*) entity->getComponent(RigidBodyEComp::sComponentID);
    
    btTransform trans;
    trans.setIdentity();
    trans.setOrigin(comp->mArgInitialLoc);
    comp->mMotionState = new RigidBodyMotionListener(trans, comp);
    btVector3 inertia(0, 0, 0);
    comp->mCollisionShape->calculateLocalInertia(comp->mMass, inertia);
    comp->mRigidBody = new btRigidBody(comp->mMass, comp->mMotionState, comp->mCollisionShape, inertia);
    comp->mRigidBody->setUserPointer(entity);
    mDynamicsWorld->addRigidBody(comp->mRigidBody);
    
    mTrackedEntities.push_back(entity);
}
void RigidBodyESys::onEntityDestroyed(nres::Entity* entity) {
    RigidBodyEComp* comp = (RigidBodyEComp*) entity->getComponent(RigidBodyEComp::sComponentID);
    
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
        RigidBodyEComp* rigidBody = (RigidBodyEComp*) entity->getComponent(RigidBodyEComp::sComponentID);
        
        if(rigidBody->mOnPhysUpdate) {
            entity->broadcast(new PhysicsLocationUpdateESignal(rigidBody->mLocation));
            entity->broadcast(new PhysicsOrientationUpdateESignal(rigidBody->mRotation));
            
            rigidBody->mOnPhysUpdate = false;
        }
    }
}

}

