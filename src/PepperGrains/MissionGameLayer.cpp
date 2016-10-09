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

#include "MissionGameLayer.hpp"

#include <chrono>
#include <thread>

namespace pgg
{

MissionGameLayer::MissionGameLayer(uint32_t width, uint32_t height)
: mScreenWidth(width)
, mScreenHeight(height) {
}

MissionGameLayer::~MissionGameLayer()
{
}

// Lifecycle
void MissionGameLayer::onBegin() {
    mSmac = new SmacRenderer(mScreenWidth, mScreenHeight);
    mSmac->grab();
}
void MissionGameLayer::onEnd() {
    mSmac->drop();
}

// Ticks
void MissionGameLayer::onTick(float tpf, const InputState* keyStates) {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    
    mSmac->renderFrame();
}

}

