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

#include "Resource.hpp"

#include <cassert>

#include "Addons.hpp"

namespace pgg {

Resource::Resource(Type resourceType)
: mResourceType(resourceType) { }
Resource::~Resource() { }

void Resource::setFile(boost::filesystem::path file) {
    assert(mFile == boost::filesystem::path() && "File loading path is set twice.");
    mFile = file;
}
boost::filesystem::path Resource::getFile() const {
    return mFile;
}
void Resource::setName(std::string name) {
    mName = name;
}
std::string Resource::getName() const {
    return mName;
}
void Resource::setSize(uint32_t size) {
    mFileSize = size;
}
uint32_t Resource::getSize() const {
    return mFileSize;
}
void Resource::setAddon(Addons::Addon* addon) {
    mAddon = addon;
}
Addons::Addon* Resource::getAddon() const {
    return mAddon;
}

}
