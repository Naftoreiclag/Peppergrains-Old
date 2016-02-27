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

#ifndef VSE_GAMELAYER_HPP
#define VSE_GAMELAYER_HPP

#include "SDL2/SDL_events.h"

#include "GameLayerMachine.hpp"

namespace pgg {

class GameLayer {
public:
    GameLayer();
    virtual ~GameLayer();
public:
    // Lifecycle
    virtual void onBegin();
    virtual void onEnd();
    
    // Ticks
    virtual void onTick(float tps, const Uint8* keyStates);
    
    /* Key filtering:
     *  Set whatever keystates to be false before passing them on to the next layers
     *  Return true to set all keys to false
     *  Not guaranteed to be called each tick
     */
    virtual bool filterKeys(Uint8* keyStates);
    
    // Layering
    virtual void onAddedAbove(const GameLayer* layer);
    virtual void onRemovedAbove(const GameLayer* layer);
    
    // Key handling
    virtual bool onKeyPress(const SDL_KeyboardEvent& event, bool repeat);
    virtual bool onKeyRelease(const SDL_KeyboardEvent& event);
    virtual bool onTextInput(const SDL_TextInputEvent& event);
    
    // Mouse handling
    virtual bool onMouseMove(const SDL_MouseMotionEvent& event);
    virtual bool onMousePress(const SDL_MouseButtonEvent& event);
    virtual bool onMouseRelease(const SDL_MouseButtonEvent& event);
    virtual bool onMouseWheel(const SDL_MouseWheelEvent& event);
};

}

#endif // VSE_GAMELAYER_HPP
