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

namespace pgg {

Resource::Resource() { }
Resource::~Resource() { }

bool Resource::isFallback() const {
    return mFile == boost::filesystem::path();
}

void Resource::setFile(const boost::filesystem::path& file) {
    assert(mFile == boost::filesystem::path() && "File loading path is set twice.");
    mFile = file;
}
const boost::filesystem::path& Resource::getFile() {
    return mFile;
}
void Resource::setName(std::string name) {
    mName = name;
}
const std::string& Resource::getName() {
    return mName;
}
void Resource::setSize(uint32_t size) {
    mFileSize = size;
}
const uint32_t& Resource::getSize() {
    return mFileSize;
}

}
