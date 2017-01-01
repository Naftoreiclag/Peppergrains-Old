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

#include "ScriptResource.hpp"

#include <fstream>
#include <iostream>
#include <sstream>

#include "Logger.hpp"

namespace pgg {

ScriptResource::ScriptResource()
: Resource(Resource::Type::SCRIPT) {
}

ScriptResource::~ScriptResource() {
}

ScriptResource* ScriptResource::gallop(Resource* resource) {
    if(!resource || resource->mResourceType != Resource::Type::SCRIPT) {
        Logger::log(Logger::WARN) << "Failed to cast " << (resource ? resource->getName() : "nullptr") << " to script!" << std::endl;
        return nullptr;
    } else {
        return static_cast<ScriptResource*>(resource);
    }
}

void ScriptResource::load() {
    //assert(this->getAddon() && "Script resource load attempted with no associated addon!");
    //assert(this->getAddon()->mLuaEnv != Scripts::EMPTY_REF && "Script resource load attempted with no environment!");
    assert(mFunc == Scripts::REF_EMPTY && "Script resource already has function loaded!");
    assert(!mLoaded && "Attempted to load script that is already loaded");
    
    mFunc = Scripts::loadFunc(this->getFile().string().c_str(), mEnv);
    mLoaded = true;
}
void ScriptResource::unload() {
    Scripts::unref(mFunc);
    mLoaded = false;
}
Scripts::CallStat ScriptResource::run() {
    assert(mLoaded && "Attempted to call method before loading");
    Scripts::pushRef(mFunc);
    return Scripts::popCallFuncArgs();
}
void ScriptResource::setEnv(Scripts::RegRef env) {
    mEnv = env;
    
    if(mLoaded) {
        Scripts::pushRef(mFunc);
        Scripts::setEnv(mEnv);
        Scripts::pop();
    }
}

}
