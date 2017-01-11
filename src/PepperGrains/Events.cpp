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

#include "Events.hpp"

namespace pgg {

#ifdef PGG_SDL
QuitEvent::QuitEvent(SDL_QuitEvent e) { }

TextInputEvent::TextInputEvent(SDL_TextInputEvent e)
: text(e.text) { }

KeyboardEvent::KeyboardEvent(SDL_KeyboardEvent e)
: pressed(e.state == SDL_PRESSED)
, repeat(e.repeat)
, key(Input::scancodeFromSDLKeyScancode(e.keysym.scancode)) { }

MouseMoveEvent::MouseMoveEvent(SDL_MouseMotionEvent e)
: x(e.x)
, y(e.y)
, dx(e.xrel)
, dy(e.yrel) { }

MouseButtonEvent::MouseButtonEvent(SDL_MouseButtonEvent e)
: button(Input::scancodeFromSDLMouse(e.button))
, pressed(e.state == SDL_PRESSED)
, x(e.x)
, y(e.y) { }
//, clicks(e.clicks) 

MouseWheelMoveEvent::MouseWheelMoveEvent(SDL_MouseWheelEvent e) {
    if(e.direction == SDL_MOUSEWHEEL_FLIPPED) {
        x = -e.x;
        y = -e.y;
    } else {
        x = e.x;
        y = e.y;
    }
}

/*
WindowResizeEvent::WindowResizeEvent(SDL_WindowEvent e)
: width(e.data1 < 1 ? 1 : e.data1)
, height(e.data2 < 1 ? 1 : e.data2) { }
*/
#endif

#ifdef PGG_GLFW
QuitEvent::QuitEvent() { }

/*
TextInputEvent::TextInputEvent(SDL_TextInputEvent e)
: text(e.text) { }
*/

KeyboardEvent::KeyboardEvent(int key, int action)
: pressed(action != GLFW_RELEASE)
, repeat(action == GLFW_REPEAT)
, key(Input::scancodeFromGLFWKey(key)) { }

MouseMoveEvent::MouseMoveEvent(double x, double y, double dx, double dy)
: x(x)
, y(y)
, dx(dx)
, dy(dy) { }

MouseButtonEvent::MouseButtonEvent(int button, int status, double x, double y)
: button(Input::scancodeFromGLFWMouse(button))
, pressed(status == GLFW_PRESS)
, x(x)
, y(y) { }

MouseWheelMoveEvent::MouseWheelMoveEvent(double dx, double dy)
: x(dx)
, y(dy) { }
#endif // PGG_GLFW


WindowResizeEvent::WindowResizeEvent(uint32_t width, uint32_t height)
: width(width < 1 ? 1 : width)
, height(width < 1 ? 1 : height) { }

}

