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

#include "ScriptResource.hpp"

#include <fstream>
#include <iostream>
#include <sstream>

namespace pgg {

ScriptResource::ScriptResource()
: mLoaded(false) {
}

ScriptResource::~ScriptResource() {
}

void ScriptResource::load() {
    if(!mLoaded) {
        std::ifstream loader(this->getFile().string().c_str());
        std::stringstream ss;
        ss << loader.rdbuf();
        loader.close();

        mString = ss.str();
        mLoaded = true;
    }
}

void ScriptResource::unload() {
}

const std::string& ScriptResource::getString() {
    return mString;
}

}
