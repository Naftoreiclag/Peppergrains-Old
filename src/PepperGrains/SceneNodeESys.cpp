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

#include "SceneNodeESys.hpp"

#include <iostream>

#include "SceneNodeEComp.hpp"
#include "PhysicsLocationUpdateESignal.hpp"
#include "PhysicsOrientationUpdateESignal.hpp"

namespace pgg {

SceneNodeESys::SceneNodeESys(SceneNode* rootNode)
: mRootNode(rootNode) {
    mRequiredComponents.push_back(SceneNodeEComp::sComponentID);
}

SceneNodeESys::~SceneNodeESys() {
    
}

void SceneNodeESys::onEntityExists(nres::Entity* entity) {
    SceneNodeEComp* comp = (SceneNodeEComp*) entity->getComponent(SceneNodeEComp::sComponentID);
    comp->mSceneNode = mRootNode->newChild();
    comp->mSceneNode->grab();
    if(comp->mArgModel) {
        comp->mSceneNode->grabModel(comp->mArgModel);
    }
}
void SceneNodeESys::onEntityDestroyed(nres::Entity* entity) {
    SceneNodeEComp* comp = (SceneNodeEComp*) entity->getComponent(SceneNodeEComp::sComponentID);
    comp->mSceneNode->drop();
}
void SceneNodeESys::onEntityBroadcast(nres::Entity* entity, const ESignal* data) {
    ESignal::Type type = data->getType();
    
    if(type != ESignal::Type::PHYSICS_LOCATION && type != ESignal::Type::PHYSICS_ORIENTATION) {
        return;
    }
    
    SceneNodeEComp* comp = (SceneNodeEComp*) entity->getComponent(SceneNodeEComp::sComponentID);
    
    switch(data->getType()) {
        case ESignal::Type::PHYSICS_LOCATION: {
            const PhysicsLocationUpdateESignal* physUpdate = (const PhysicsLocationUpdateESignal*) data;
            comp->mSceneNode->setLocalTranslation(physUpdate->mAbsoluteLocation);
            break;
        }
        case ESignal::Type::PHYSICS_ORIENTATION: {
            const PhysicsOrientationUpdateESignal* physUpdate = (const PhysicsOrientationUpdateESignal*) data;
            comp->mSceneNode->setLocalOrientation(physUpdate->mOrientation);
            break;
        }
        default: break;
    }
}

const std::vector<nres::ComponentID>& SceneNodeESys::getRequiredComponents() {
    return mRequiredComponents;
}

void SceneNodeESys::onTick(const float& tps) {
    
}

}
