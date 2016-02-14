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

#include "FontResource.hpp"

#include <cassert>
#include <iostream>
#include <fstream>

#include "ResourceManager.hpp"
#include "StreamStuff.hpp"

namespace pgg {

FontResource::FontResource()
: mLoaded(false) {
}

FontResource::~FontResource() {
}

bool FontResource::load() {
    if(mLoaded) {
        return true;
    }

    std::ifstream input(this->getFile().c_str(), std::ios::in | std::ios::binary);

    std::string imageName;
    readString(input, imageName);

    ResourceManager* rmgr = ResourceManager::getSingleton();

    mImage = rmgr->findImage(imageName);
    mImage->grab();

    return true;
}
bool FontResource::unload() {
    assert(mLoaded && "Attempted to unload font before loading it");

    mImage->drop();

    mLoaded = false;
    return true;
}

}
