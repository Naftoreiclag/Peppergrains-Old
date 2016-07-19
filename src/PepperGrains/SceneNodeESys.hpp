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

#ifndef PGG_SCENENODEESYS_HPP
#define PGG_SCENENODEESYS_HPP

#include "NRES.hpp" // Base class: nres::System

#include "SceneNode.hpp"

namespace pgg {

class SceneNodeESys : public nres::System {
public:
    SceneNodeESys(SceneNode* rootNode);
    ~SceneNodeESys();
private:
    std::vector<nres::ComponentID> mRequiredComponents;
    std::vector<nres::Entity*> mTrackedEntities;
public:
    SceneNode* const mRootNode;
    
    void onEntityExists(nres::Entity* entity);
    void onEntityDestroyed(nres::Entity* entity);
    void onEntityBroadcast(nres::Entity* entity, const ESignal* data);
    
    const std::vector<nres::ComponentID>& getRequiredComponents();
    
    void onTick(const float& tps);
};

}

#endif // PGG_SCENENODEESYS_HPP
