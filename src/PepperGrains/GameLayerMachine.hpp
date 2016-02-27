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

#ifndef VSE_GAMELAYERMACHINE_HPP
#define VSE_GAMELAYERMACHINE_HPP

#include "SDL2/SDL_events.h"

#include <vector>

namespace pgg {

class GameLayer;
class GameLayerMachine {
public:
    GameLayerMachine();
    ~GameLayerMachine();

private:
    std::vector<GameLayer*> mLayers;
    
    static Uint8* sRelaxedKeyStates;
    static int sNumKeyStates;
    
    Uint8* mFilteredKeyStates;
public:
    void addAbove(GameLayer* addMe, GameLayer* aboveMe);
    void addBottom(GameLayer* addMe);
    void remove(GameLayer* removeMe);
    void removeAll();
    
    // Ticks
    void onTick(float tps, const Uint8* keyStates);
    
    // Key handling
    void onKeyPress(const SDL_KeyboardEvent& event, bool repeat);
    void onKeyRelease(const SDL_KeyboardEvent& event);
    void onTextInput(const SDL_TextInputEvent& event);
    
    // Mouse handling
    void onMouseMove(const SDL_MouseMotionEvent& event);
    void onMousePress(const SDL_MouseButtonEvent& event);
    void onMouseRelease(const SDL_MouseButtonEvent& event);
    void onMouseWheel(const SDL_MouseWheelEvent& event);
    
    const Uint8* getRelaxedKeyStates();
};

}

#endif // VSE_GAMELAYERMACHINE_HPP
