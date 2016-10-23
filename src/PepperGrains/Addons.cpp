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

#include "Addons.hpp"

namespace pgg {
namespace Addons {

    /* To prevent errors occuring due to arbitrary load order, all addons which are eligible to
     * be loaded at any given point in the load process are loaded "together," i.e. as if they
     * were all loading at the same time on different threads.
     * 
     * This method simulates this effect by loading the addons sequentially, but keeping track of
     * the resources that they modify. If multiple addons try to modify the same resource in a
     * single call of this method, both error.
     * 
     * Unfortunately, there may still be order-dependency which cannot be checked (e.g. a script
     * calling another script's function created within the same call cannot be intervened). To
     * "prevent" this, load order within this call is also randomized.
     * 
     * Error checking is also done on all addons before bootstrapping them "individually"
     */
    //void bootstrapAddonsConcurrently(std::vector<Addon*> addons, ScriptEvaluator* evalulator) {
        
    //}


} // Addons
} // pgg

