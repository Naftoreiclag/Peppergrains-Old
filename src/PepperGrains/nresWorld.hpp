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

#ifndef NRES_WORLD_HPP
#define NRES_WORLD_HPP

#include <vector>

#include "nresSystem.hpp"
#include "nresEntity.hpp"

namespace nres
{

class World {
public:
    World();
    ~World();
private:
    std::vector<System*> systems;
    std::vector<Entity*> entities;

public:
    void attachSystem(System* system);
    
    Entity* newEntity();
    void deleteEntity(Entity* entity);
    void deleteAllEntities();
    
    const std::vector<Entity*>& getEntities() const;
    
    friend class Entity;
};

}

#endif // NRES_WORLD_HPP
