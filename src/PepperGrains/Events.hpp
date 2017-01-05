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

#ifndef PGG_EVENTS_HPP
#define PGG_EVENTS_HPP

#include <stdint.h>
#include <string>

#include <WindowInputSystemStuff.hpp>

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
    MouseMoveEvent(double x, double y, double dx, double dy);
    #endif
    
    double x;
    double y;
    double dx;
    double dy;
};

struct MouseButtonEvent {
    #ifdef PGG_SDL
    MouseButtonEvent(SDL_MouseButtonEvent e);
    #endif
    #ifdef PGG_GLFW
    MouseButtonEvent(int button, int status, double x, double y);
    #endif
    
    Input::Scancode button;
    bool pressed;
    double x;
    double y;
};

struct MouseWheelMoveEvent {
    #ifdef PGG_SDL
    MouseWheelMoveEvent(SDL_MouseWheelEvent e);
    #endif
    #ifdef PGG_GLFW
    MouseWheelMoveEvent(double dx, double dy);
    #endif
    
    double x;
    double y;
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

#endif // PGG_EVENTS_HPP
