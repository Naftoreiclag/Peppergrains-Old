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

#ifndef PGG_SCRIPTRESOURCE_HPP
#define PGG_SCRIPTRESOURCE_HPP

#include "Resource.hpp"
#include "Scripts.hpp"

namespace pgg {

class ScriptResource : public Resource {
private:
    Scripts::RegRef mFunc = Scripts::REF_EMPTY;
    Scripts::RegRef mEnv = Scripts::REF_EMPTY;
    
    bool mLoaded = false;
    
public:
    ScriptResource();
    ~ScriptResource();
    static ScriptResource* upcast(Resource* resource);
    
    void setEnv(Scripts::RegRef env); // This can be called before or after loading the script (before using r->grab())
    
    void load();
    void unload();
    
    Scripts::CallStat run();
};

}

#endif // SCRIPTRESOURCE_HPP
