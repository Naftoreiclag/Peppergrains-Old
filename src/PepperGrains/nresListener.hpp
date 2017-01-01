/*
   Copyright 2015-2017 James Fong

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

#ifndef NRES_LISTENER_HPP
#define NRES_LISTENER_HPP

#include "nresTypedefs.hpp"

namespace nres {
    
class Entity;
class Listener {
public:
    Listener();
    virtual ~Listener();
public:
    virtual void onEntityExists(Entity* entity) = 0;
    virtual void onEntityDestroyed(Entity* entity) = 0;
    virtual void onEntityBroadcast(Entity* entity, const EntitySignal* data) = 0;
};

}

#endif // NRES_LISTENER_HPP
