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

#include "SoundReceiver.hpp"
#include "SoundContext.hpp"
#include "SoundEndpoint.hpp"

namespace pgg {
namespace Sound {

Receiver::Receiver(Context* context, Endpoint* endpoint)
: mContext(context)
, mRequestedContext(nullptr)
, mEndpoint(endpoint)
, mRequestedEndpoint(nullptr) {
    if(mEndpoint) {
        mEndpoint->grabReciever(this);
    }
    if(mContext) {
        
    }
}

Receiver::~Receiver() {
}

void Receiver::unsetContext() { setContext(nullptr); }
void Receiver::setContext(Context* context) {
    if(mContext == context) { return; }
    if(mContext) {
        mRequestedContext = context;
    } else {
        mContext = context;
        if(mContext) {
            
        }
    }
}

void Receiver::unsetEndpoint() { setEndpoint(nullptr); }
void Receiver::setEndpoint(Endpoint* endpoint) {
    if(mEndpoint == endpoint) { return; }
    if(mEndpoint) {
        mRequestedEndpoint = endpoint;
    } else {
        mEndpoint = endpoint;
        if(mEndpoint) {
            mEndpoint->grabReciever(this);
        }
    }
}

void Receiver::updateCalc(double time, Endpoint* endpnt) {
    mContext->updateCalc(time, endpnt, this);
}

void Receiver::load() {
    
}
void Receiver::unload() {
    
}

} // namespace Sound
} // namespace pgg
