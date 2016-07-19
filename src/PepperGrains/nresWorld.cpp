/*
   Copyright 2015-2016 James Fong

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

#include "nresWorld.hpp"

#include <algorithm>

namespace nres {

World::World() {
}

World::~World() {
}

void World::attachSystem(System* system) {
    systems.push_back(system);
}

Entity* World::newEntity() {
    Entity* ret = new Entity(this);
    
    entities.push_back(ret);
    
    return ret;
}

void World::deleteEntity(Entity* entity) {
    entities.erase(std::remove(entities.begin(), entities.end(), entity), entities.end());
    
    delete entity;
}

void World::deleteAllEntities() {
    for(std::vector<Entity*>::iterator entIter = entities.begin(); entIter != entities.end(); ++ entIter) {
        Entity* entity = *entIter;
        
        delete entity;
    }
    
    entities.clear();
}

const std::vector<Entity*>& World::getEntities() const {
    return entities;
}

}

