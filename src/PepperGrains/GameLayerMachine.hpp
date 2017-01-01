/*
   Copyright 2015-2017 James Fong

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

#include <vector>

#include "NALEvents.hpp"

namespace pgg {

class Gamelayer;
class GamelayerMachine {
public:
    GamelayerMachine();
    ~GamelayerMachine();

private:
    std::vector<Gamelayer*> mLayers;
    
    static InputState sEmptyInputState;
    
public:
    void addAbove(Gamelayer* addMe, Gamelayer* aboveMe);
    void addBottom(Gamelayer* addMe);
    void remove(Gamelayer* removeMe);
    void removeAll();
    
    // Ticks
    void onTick(float tpf, const InputState* inputState);
    
    // Key handling
    void onKeyboardEvent(const KeyboardEvent& event);
    void onTextInput(const TextInputEvent& event);
    
    // Mouse handling
    void onMouseMove(const MouseMoveEvent& event);
    void onMouseButton(const MouseButtonEvent& event);
    void onMouseWheel(const MouseWheelMoveEvent& event);
    
    void onWindowSizeUpdate(const WindowResizeEvent& event);
    
    void onQuit(const QuitEvent& event);
};

}

#endif // VSE_GAMELAYERMACHINE_HPP
