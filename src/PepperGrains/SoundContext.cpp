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

#include "SoundContext.hpp"

#include <algorithm>

namespace pgg {
namespace Sound {

Context::Context(float machSpeed)
: mMachSpeed(machSpeed) {
}

Context::~Context() {
}

void Context::updateCalc(double time, Endpoint* endpnt, Receiver* receiver) {
    for(std::vector<Source*>::iterator iter = mSources.begin(); iter != mSources.end(); ++ iter) {
        Source* source = *iter;
        
        //source->updateCalc(time, Endpoint* endpnt, Receiver* receiver);
    }
}

Source* Context::grabSource(Source* source) {
    assert(std::find(mSources.begin(), mSources.end(), source) == mSources.end() && 
        "Attempted to add a single sound source multiple times to the same sound context");
    
    mSources.push_back(source);
    source->grab();
    return source;
}
void Context::dropSource(Source* source) {
    assert(std::find(mSources.begin(), mSources.end(), source) != mSources.end() && 
        "Attempted to remove sound source from sound context which did not already have that source");
    
    mSources.erase(std::remove(mSources.begin(), mSources.end(), source), mSources.end());
    source->drop();
}
void Context::dropAllSources() {
    for(std::vector<Source*>::iterator iter = mSources.begin(); iter != mSources.end(); ++ iter) {
        Source* source = *iter;
        source->drop();
    }
    
    mSources.clear();
}

// TODO: assert that grabbed source is not already in vector
void Context::grabMedium(Medium* medium) {
    
}
void Context::dropMedium(Medium* medium) {
    
}
void Context::dropAllMedia() {
    
}

void Context::load() {
    
}

void Context::unload() {
    
}

} // namespace Sound
} // namespace pgg

