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

#include "nresEntity.hpp"

#include <iostream>
#include <cassert>

#include "nresWorld.hpp"
#include "nresComponent.hpp"

namespace nres {

Entity::Entity(World* world)
: world(world)
, isPublished(false) {
}

Entity::~Entity() {
    // Broadcast deletion to every listener
    for(std::vector<Listener*>::iterator listIter = listeners.begin(); listIter != listeners.end(); ++ listIter) {
        Listener* listener = *listIter;
        listener->onEntityDestroyed(this);
    }
    
    for(std::vector<Component*>::iterator compIter = components.begin(); compIter != components.end(); ++ compIter) {
        Component* comp = *compIter;
        delete comp;
    }
}

void Entity::add(Component* component) {
    assert(!isPublished);
    
    components.push_back(component);
}

void Entity::addListener(Listener* listener) {
    assert(!isPublished);
    
    listeners.push_back(listener);
}

void Entity::destroy() {
    assert(isPublished);
    
    world->deleteEntity(this);
}

void Entity::publish() {
    assert(!isPublished);
    isPublished = true;
    
    // Try find which listeners (systems) correspond with this entity's set of components
    for(std::vector<System*>::iterator sysIter = world->systems.begin(); sysIter != world->systems.end(); ++ sysIter) {
        System* sys = *sysIter;
        
        // Iterate over that system's requirements, check that each one is fulfilled
        bool haveAllRequirements = true;
        const std::vector<ComponentID>& requirements = sys->getRequiredComponents();
        for(std::vector<ComponentID>::const_iterator reqIter = requirements.begin(); reqIter != requirements.end(); ++ reqIter) {
            const ComponentID& reqId = *reqIter;
            
            // Check if we fulfill this requirements
            bool haveRequirement = false;
            for(std::vector<Component*>::iterator compIter = components.begin(); compIter != components.end(); ++ compIter) {
                Component* myComponent = *compIter;
                
                if(myComponent->getID() == reqId) {
                    haveRequirement = true;
                    break;
                }
            }
            
            // If we are missing a single requirement, then we do not have all of them
            if(!haveRequirement) {
                haveAllRequirements = false;
                break;
            }
        }
        
        // Entity has all requirements, therefore add it to our listener list
        if(haveAllRequirements) {
            listeners.push_back(sys);
        }
    }
    
    // Tell every listener that we exist
    for(std::vector<Listener*>::iterator listIter = listeners.begin(); listIter != listeners.end(); ++ listIter) {
        Listener* listener = *listIter;
        listener->onEntityExists(this);
    }
}

Component* Entity::getComponent(const ComponentID& componentID) {
    assert(isPublished);
    
    for(std::vector<Component*>::iterator compIter = components.begin(); compIter != components.end(); ++ compIter) {
        Component* comp = *compIter;
        
        if(comp->getID() == componentID) {
            return comp;
        }
    }
    
    return 0;
}

void Entity::broadcast(EntitySignal* data) {
    assert(isPublished);
    
    // Broadcast message to every listener
    for(std::vector<Listener*>::iterator listIter = listeners.begin(); listIter != listeners.end(); ++ listIter) {
        Listener* listener = *listIter;
        listener->onEntityBroadcast(this, data);
    }
    
    delete data;
}

const std::vector<Component*>& Entity::getComponents() const {
    return components;
}

}

