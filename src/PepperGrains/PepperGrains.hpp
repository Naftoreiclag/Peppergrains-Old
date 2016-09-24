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

#ifndef PGG_PEPPERGRAINS_HPP
#define PGG_PEPPERGRAINS_HPP

#include "soundio/soundio.h"
#include "lua.hpp"

#include "GameLayerMachine.hpp"
#include "SoundEndpoint.hpp"
#include "ResourceManager.hpp"

namespace pgg {
    
class BootstrapScriptEval : public ResourceManager::ScriptEvaluator {
public:
    void execute(const ResourceManager::Addon* addon, ScriptResource* bootScript);
};

class PepperGrains {
public:
    static PepperGrains* getSingleton();
public:
    PepperGrains();
    ~PepperGrains();
    
private:
    SoundIo* mSndIo;
    SoundIoDevice* mSndDevice;
    
    BootstrapScriptEval bootstrapScriptEval;
    
    float mTps;
    float mTpsWeight;
    float mOneSecondTimer;
    
public:
    Sound::Endpoint* mSndEndpoint;

    bool mMainLoopRunning;
    GameLayerMachine mGameLayerMachine;
    
    uint32_t getRunningTimeMilliseconds() const;
    double getRunningTimeSeconds() const;
    
    
    int run(int argc, char* argv[]);
    
    
};

}

#endif // PGG_PEPPERGRAINS_HPP
