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

#ifndef PGG_NALEVENTS_HPP
#define PGG_NALEVENTS_HPP

// NAL = Naftoreiclag's Abstraction Layer

#include <stdint.h>
#include <string>

#include "SDL2/SDL.h"

#include "NALInputs.hpp"

namespace pgg {

struct QuitEvent {
    QuitEvent(SDL_QuitEvent e);
};

struct TextInputEvent {
    TextInputEvent(SDL_TextInputEvent e);
    
    std::string text;
};

struct KeyboardEvent {
    KeyboardEvent(SDL_KeyboardEvent e);
    
    // true = pressed, false = released
    bool pressed;
    uint8_t repeat;
    SDL_Keysym symbol;
};

struct MouseMoveEvent {
    MouseMoveEvent(SDL_MouseMotionEvent e);
    
    int32_t x;
    int32_t y;
    int32_t dx;
    int32_t dy;
};

struct MouseButtonEvent {
    MouseButtonEvent(SDL_MouseButtonEvent e);
    
    Input::MouseButton button;
    bool pressed;
    uint8_t clicks;
    int32_t x;
    int32_t y;
};

struct MouseWheelMoveEvent {
    MouseWheelMoveEvent(SDL_MouseWheelEvent e);
    
    int32_t x;
    int32_t y;
    bool flipped;
};

struct WindowResizeEvent {
    WindowResizeEvent(SDL_WindowEvent e);
    
    int32_t width;
    int32_t height;
};


}

#endif // PGG_NALEVENTS_HPP
