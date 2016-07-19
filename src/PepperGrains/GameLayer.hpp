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

#include "NALEvents.hpp"

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
    virtual void onTick(float tpf, const InputState* keyStates);
    
    /* Key filtering:
     *  Set whatever keystates to be false before passing them on to the next layers
     *  Return true to set all keys to false
     *  Not guaranteed to be called each tick
     */
    virtual bool filterKeys(InputState* keyStates);
    
    // Layering
    virtual void onAddedAbove(const GameLayer* layer);
    virtual void onRemovedAbove(const GameLayer* layer);
    
    // Key handling
    virtual bool onKeyboardEvent(const KeyboardEvent& event);
    virtual bool onTextInput(const TextInputEvent& event);
    
    // Mouse handling
    virtual bool onMouseMove(const MouseMoveEvent& event);
    virtual bool onMouseButton(const MouseButtonEvent& event);
    virtual bool onMouseWheel(const MouseWheelMoveEvent& event);
    
    // Windowing
    virtual bool onWindowSizeUpdate(const WindowResizeEvent& event);
    
    virtual bool onQuit(const QuitEvent& event);
};

}

#endif // VSE_GAMELAYER_HPP
