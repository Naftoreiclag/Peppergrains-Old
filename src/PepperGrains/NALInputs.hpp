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

#ifndef PGG_NALINPUTS_HPP
#define PGG_NALINPUTS_HPP

#include "SDL2/SDL.h"

namespace pgg {

namespace Input {

enum Scancode {
    UNKNOWN,
    
    M_LEFT,
    M_MIDDLE,
    M_RIGHT,
    M_SPECIAL_0,
    M_SPECIAL_1,
    K_0,
    K_1,
    K_2,
    K_3,
    K_4,
    K_5,
    K_6,
    K_7,
    K_8,
    K_9,
    K_A,
    K_B,
    K_C,
    K_D,
    K_E,
    K_F,
    K_G,
    K_H,
    K_I,
    K_J,
    K_K,
    K_L,
    K_M,
    K_N,
    K_O,
    K_P,
    K_Q,
    K_R,
    K_S,
    K_T,
    K_U,
    K_V,
    K_W,
    K_X,
    K_Y,
    K_Z,
    K_NUMPAD_0,
    K_NUMPAD_1,
    K_NUMPAD_2,
    K_NUMPAD_3,
    K_NUMPAD_4,
    K_NUMPAD_5,
    K_NUMPAD_6,
    K_NUMPAD_7,
    K_NUMPAD_8,
    K_NUMPAD_9,
    K_NUMPAD_00,
    K_NUMPAD_000,
    K_NUMPAD_A,
    K_NUMPAD_B,
    K_NUMPAD_C,
    K_NUMPAD_D,
    K_NUMPAD_E,
    K_NUMPAD_F,
    K_NUMPAD_AMPERSAND,
    K_NUMPAD_AT,
    K_NUMPAD_BACKSPACE,
    K_NUMPAD_BINARY,
    K_NUMPAD_CLEAR,
    K_NUMPAD_CLEAR_ENTRY,
    K_NUMPAD_COLON,
    K_NUMPAD_COMMA,
    K_NUMPAD_DOUBLE_AMPERSAND,
    K_NUMPAD_DOUBLE_VERTICAL,
    K_NUMPAD_DECIMAL_POINT,
    K_NUMPAD_DIVIDE,
    K_NUMPAD_ENTER,
    K_NUMPAD_EQUALS,
    K_NUMPAD_EQUALS_ALT,
    K_NUMPAD_FACTORIAL,
    K_NUMPAD_GREATER_THAN,
    K_NUMPAD_HASH,
    K_NUMPAD_HEX,
    K_NUMPAD_BRACE_LEFT,
    K_NUMPAD_PARENTHESIS_LEFT,
    K_NUMPAD_LESS_THAN,
    K_NUMPAD_MEMORY_ADD,
    K_NUMPAD_MEMORY_CLEAR,
    K_NUMPAD_MEMORY_DIVIDE,
    K_NUMPAD_MEMORY_MULTIPLY,
    K_NUMPAD_MEMORY_CALL,
    K_NUMPAD_MEMORY_STORE,
    K_NUMPAD_MEMORY_SUBTRACT,
    K_NUMPAD_SUBTRACT,
    K_NUMPAD_MULTIPLY,
    K_NUMPAD_OCT,
    K_NUMPAD_PERCENT,
    K_NUMPAD_PERIOD,
    K_NUMPAD_ADD,
    K_NUMPAD_PLUS_MINUS,
    K_NUMPAD_CARET,
    K_NUMPAD_BRACE_RIGHT,
    K_NUMPAD_PARENTHESIS_RIGHT,
    K_NUMPAD_SPACE,
    K_NUMPAD_TAB,
    K_NUMPAD_VERTICAL,
    K_NUMPAD_XOR,
    K_F1,
    K_F2,
    K_F3,
    K_F4,
    K_F5,
    K_F6,
    K_F7,
    K_F8,
    K_F9,
    K_F10,
    K_F11,
    K_F12,
    K_F13,
    K_F14,
    K_F15,
    K_F16,
    K_F17,
    K_F18,
    K_F19,
    K_F20,
    K_F21,
    K_F22,
    K_F23,
    K_F24,
    K_APP_BACK,
    K_APP_FAVORITES,
    K_APP_FORWARD,
    K_APP_HOME,
    K_APP_REFRESH,
    K_APP_SEARCH,
    K_APP_STOP,
    K_REDO,
    K_ERASE,
    K_APOSTROPHE,
    K_APP_MENU,
    K_SND_MUTE,
    K_SND_NEXT,
    K_SND_PLAY,
    K_SND_PREV,
    K_SND_STOP,
    K_BACKSLASH,
    K_BACKSPACE,
    K_BRIGHTNESS_DOWN,
    K_BRIGHTNESS_UP,
    K_CALCULATOR,
    K_CANCEL,
    K_CAPS_LOCK,
    K_CLEAR,
    K_CLEAR_AGAIN,
    K_COMMA,
    K_MY_COMPUTER,
    K_COPY,
    K_CR_CEL,
    K_CURRENCY_SUB_UNIT,
    K_CURRENCY_UNIT,
    K_CUT,
    K_DECIMAL_POINT,
    K_DELETE,
    K_VIDEO_MODE_SWITCH,
    K_ARROW_DOWN,
    K_EJECT,
    K_END,
    K_EQUALS,
    K_ESCAPE,
    K_EXECUTE,
    K_EX_SEL,
    K_FIND,
    K_GRAVE,
    K_HELP,
    K_HOME,
    K_INSERT,
    K_INTERNATIONAL_1,
    K_INTERNATIONAL_2,
    K_INTERNATIONAL_3,
    K_INTERNATIONAL_4,
    K_INTERNATIONAL_5,
    K_INTERNATIONAL_6,
    K_INTERNATIONAL_7,
    K_INTERNATIONAL_8,
    K_INTERNATIONAL_9,
    K_ILLUMINATION_DOWN,
    K_ILLUMINATION_TOGGLE,
    K_ILLUMINATION_UP,
    K_ALT_LEFT,
    K_LANG_1,
    K_LANG_2,
    K_LANG_3,
    K_LANG_4,
    K_LANG_5,
    K_LANG_6,
    K_LANG_7,
    K_LANG_8,
    K_LANG_9,
    K_CONTROL_LEFT,
    K_ARROW_LEFT,
    K_BRACKET_LEFT,
    K_GUI_LEFT,
    K_SHIFT_LEFT,
    K_MAIL,
    K_MEDIA_SELECT,
    K_MENU,
    K_SUBTRACT,
    K_MODE_SWITCH,
    K_MUTE,
    K_NUMLOCK,
    K_OPER,
    K_OUT,
    K_PAGE_DOWN,
    K_PAGE_UP,
    K_PASTE,
    K_PAUSE,
    K_PERIOD,
    K_POWER,
    K_PRINTSCREEN,
    K_PRIOR,
    K_ALT_RIGHT,
    K_CONTROL_RIGHT,
    K_RETURN,
    K_RETURN_ALT,
    K_GUI_RIGHT,
    K_ARROW_RIGHT,
    K_BRACKET_RIGHT,
    K_SHIFT_RIGHT,
    K_SCROLL_LOCK,
    K_SELECT,
    K_SEMICOLON,
    K_SEPARATOR,
    K_SLASH,
    K_SLEEP,
    K_SPACEBAR,
    K_STOP,
    K_SYS_REQ,
    K_TAB,
    K_THOUSANDS_SEPARATOR,
    K_UNDO,
    K_ARROW_UP,
    K_SND_VOLUME_DOWN,
    K_SND_VOLUME_UP,
    K_WORLD_WIDE_WEB,
    
    ENUM_SIZE // Hold size of this enum; do not place enums after this point!
};

Scancode scancodeFromSDLKeyScancode(SDL_Scancode key);
Scancode scancodeFromSDLMouse(uint8_t button);

}

class InputState {
private:
    bool mPressed[Input::Scancode::ENUM_SIZE];
    
    int32_t mMouseX;
    int32_t mMouseY;
    int32_t mMouseDX;
    int32_t mMouseDY;
public:
    InputState();
    ~InputState();

    bool isPressed(Input::Scancode button) const;
    int32_t getMouseX() const;
    int32_t getMouseY() const;
    int32_t getMouseDX() const;
    int32_t getMouseDY() const;
    
    void updateKeysFromSDL();
    void updateMouseFromSDL();
    
    void setState(Input::Scancode button, bool pressed);
    void setMouseDelta(int32_t dx, int32_t dy);
};

}

#endif // PGG_NALINPUTS_HPP
