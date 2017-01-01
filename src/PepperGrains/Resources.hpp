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

#ifndef PGG_ENGINERESOURCES_HPP
#define PGG_ENGINERESOURCES_HPP

#include <string>
#include <stdint.h>

#include "Resource.hpp"

namespace pgg {
namespace Resources {

    class Modlayer {
        
    };

    void loadCore(std::string path);
    
    uint32_t getNumCoreResources();

    // Top modlayer can be edited dynamically
    void setTopModlayer(Modlayer* modlayer);
    void removeTopModlayer();
    void publishTopModlayer(); // Also removes

    void removeAllModlayers();

    Resource* find(std::string query, std::string callOrigin = "");
    
} // Resources
} // pgg

#endif // PGG_ENGINERESOURCES_HPP
