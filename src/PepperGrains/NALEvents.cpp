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

#include "NALEvents.hpp"

namespace pgg {

QuitEvent::QuitEvent(SDL_QuitEvent e) { }

TextInputEvent::TextInputEvent(SDL_TextInputEvent e)
: text(e.text) { }

KeyboardEvent::KeyboardEvent(SDL_KeyboardEvent e)
: pressed(e.state == SDL_PRESSED)
, repeat(e.repeat)
, symbol(e.keysym) { }

MouseMoveEvent::MouseMoveEvent(SDL_MouseMotionEvent e)
: x(e.x)
, y(e.y)
, dx(e.xrel)
, dy(e.yrel) { }

MouseButtonEvent::MouseButtonEvent(SDL_MouseButtonEvent e)
: button(e.button)
, pressed(e.state == SDL_PRESSED)
, clicks(e.clicks)
, x(e.x)
, y(e.x) {
}

MouseWheelMoveEvent::MouseWheelMoveEvent(SDL_MouseWheelEvent e)
: x(e.x)
, y(e.y)
, flipped(e.direction == SDL_MOUSEWHEEL_FLIPPED) { }

WindowResizeEvent::WindowResizeEvent(SDL_WindowEvent e)
: width(e.data1)
, height(e.data2) { }

}

