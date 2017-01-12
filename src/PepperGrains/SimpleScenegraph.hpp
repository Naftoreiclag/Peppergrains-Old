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

#ifndef PGG_SIMPLESCENEGRAPH_HPP
#define PGG_SIMPLESCENEGRAPH_HPP

#include "Scenegraph.hpp"

namespace pgg {
class SimpleScenegraph : public Scenegraph {
private:
    std::function<void(ModelInstance*)> mAddListener = nullptr;
    std::function<void(ModelInstance*)> mRemoveListener = nullptr;
    ModelInstance* mModelInst;
public:
    // Perform a function on all attached ModelInstances
    void processAll(std::function<void(ModelInstance*)> modelMapper);
    
    // Provide a function to be called when a new ModelInstance is added
    void setRendererAddListener(std::function<void(ModelInstance*)> modelFunc);
    
    // Provide a function to be called when a ModelInstance is removed
    void setRendererRemoveListener(std::function<void(ModelInstance*)> modelFunc);
    
    // Set the sole model instance to render; returns old one or nullptr if there was none
    ModelInstance* setModelInstance(ModelInstance* modelInst);
};

}

#endif // PGG_SIMPLESCENEGRAPH_HPP
