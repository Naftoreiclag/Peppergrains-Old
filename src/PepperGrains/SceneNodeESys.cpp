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

#include "SceneNodeComp.hpp"

namespace pgg {

SceneNodeESys::SceneNodeESys(SceneNode* rootNode)
: mRootNode(rootNode) {
    mRequiredComponents.push_back(SceneNodeComp::sComponentID);
}

SceneNodeESys::~SceneNodeESys() {
    
}

void SceneNodeESys::onEntityExists(nres::Entity* entity) {
    SceneNodeComp* comp = (SceneNodeComp*) entity->getComponent(SceneNodeComp::sComponentID);
    comp->mSceneNode = mRootNode->newChild();
    comp->mSceneNode->grab();
    comp->mSceneNode->grabModel(comp->mArgModel);
    comp->mArgModel = nullptr;
}
void SceneNodeESys::onEntityDestroyed(nres::Entity* entity) {
    SceneNodeComp* comp = (SceneNodeComp*) entity->getComponent(SceneNodeComp::sComponentID);
    comp->mSceneNode->drop();
}
void SceneNodeESys::onEntityBroadcast(nres::Entity* entity, const nres::EntitySignal* data) {
}

const std::vector<nres::ComponentID>& SceneNodeESys::getRequiredComponents() {
    return mRequiredComponents;
}

void SceneNodeESys::onTick(const float& tps) {
    
}

}
