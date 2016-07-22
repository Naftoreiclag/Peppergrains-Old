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

#include "SoundEndpoint.hpp"

#include <algorithm>

namespace pgg {
namespace Sound {

Endpoint::Endpoint()
: mDevice(nullptr) {
}

Endpoint::~Endpoint() {
}

void Endpoint::setDevice(SoundIoDevice* device) {
    mDevice = device;
}

void Endpoint::grabReciever(Receiver* receiver) {
    mReceivers.push_back(receiver);
}

void Endpoint::dropReceiver(Receiver* receiver) {
    mReceivers.erase(std::remove(mReceivers.begin(), mReceivers.end(), receiver), mReceivers.end());
    
    receiver->drop();
}

} // namespace Sound
} // namespace pgg
