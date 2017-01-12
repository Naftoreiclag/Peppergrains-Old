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

#ifndef PGG_SCENEGRAPH_HPP
#define PGG_SCENEGRAPH_HPP

#include <functional>

#include "ModelInstance.hpp"

namespace pgg {
class Scenegraph {
public:
    // Perform a function on all attached ModelInstances
    virtual void processAll(std::function<void(ModelInstance*)> modelMapper) = 0;
    
    // For the Renderer to add functions to be called when a new ModelInstance is added or removed
    virtual void setRendererAddListener(std::function<void(ModelInstance*)> modelFunc) = 0;
    virtual void setRendererRemoveListener(std::function<void(ModelInstance*)> modelFunc) = 0;
};
}

#endif // PGG_SCENEGRAPH_HPP
