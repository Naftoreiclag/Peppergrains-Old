/*
   Copyright 2016-2017 James Fong

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

#ifdef PGG_SDL
#include <SDL2/SDL.h>
#endif

#ifdef PGG_GLFW
#include <GLFW/glfw3.h>
#endif

#include "Input.hpp"

namespace pgg {

struct QuitEvent {
    #ifdef PGG_SDL
    QuitEvent(SDL_QuitEvent e);
    #endif
    #ifdef PGG_GLFW
    QuitEvent();
    #endif
};

struct TextInputEvent {
    #ifdef PGG_SDL
    TextInputEvent(SDL_TextInputEvent e);
    #endif
    #ifdef PGG_GLFW
    // TODO: IMPLEMENT
    #endif
    
    std::string text;
};

struct KeyboardEvent {
    #ifdef PGG_SDL
    KeyboardEvent(SDL_KeyboardEvent e);
    #endif
    #ifdef PGG_GLFW
    KeyboardEvent(int key, int action);
    #endif
    
    // true = pressed, false = released
    bool pressed;
    uint8_t repeat;
    Input::Scancode key;
};

struct MouseMoveEvent {
    #ifdef PGG_SDL
    MouseMoveEvent(SDL_MouseMotionEvent e);
    #endif
    #ifdef PGG_GLFW
    MouseMoveEvent(int32_t x, int32_t y, int32_t dx, int32_t dy);
    #endif
    
    int32_t x;
    int32_t y;
    int32_t dx;
    int32_t dy;
};

struct MouseButtonEvent {
    #ifdef PGG_SDL
    MouseButtonEvent(SDL_MouseButtonEvent e);
    #endif
    #ifdef PGG_GLFW
    MouseButtonEvent(int key, int status, int32_t x, int32_t, y);
    #endif
    
    Input::Scancode button;
    bool pressed;
    int32_t x;
    int32_t y;
    uint8_t clicks;
};

struct MouseWheelMoveEvent {
    #ifdef PGG_SDL
    MouseWheelMoveEvent(SDL_MouseWheelEvent e);
    #endif
    #ifdef PGG_GLFW
    #endif
    
    int32_t x;
    int32_t y;
    bool flipped;
};

struct WindowResizeEvent {
    #ifdef PGG_SDL
    WindowResizeEvent(SDL_WindowEvent e);
    #endif
    #ifdef PGG_GLFW
    #endif
    
    uint32_t width;
    uint32_t height;
};


}

#endif // PGG_NALEVENTS_HPP
