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

#ifndef PGG_MISSIONGAMELAYER_HPP
#define PGG_MISSIONGAMELAYER_HPP

#include "Gamelayer.hpp"
#include "ForwardRenderer.hpp"

namespace pgg {

class MissionGameLayer : public Gamelayer {
public:
    MissionGameLayer(uint32_t width, uint32_t height);
    ~MissionGameLayer();
private:
    uint32_t mScreenWidth;
    uint32_t mScreenHeight;
    
    ForwardRenderer* mRenderer;

public:
    // Lifecycle
    void onBegin();
    void onEnd();
    
    // Ticks
    void onTick(float tpf, const InputState* keyStates);

};

}

#endif // PGG_MISSIONGAMELAYER_HPP
