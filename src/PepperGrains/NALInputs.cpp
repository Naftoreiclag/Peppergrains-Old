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

#include "NALInputs.hpp"

#include <cassert>

namespace pgg {
    
namespace Input {


Scancode scancodeFromSDLMouse(uint8_t button) {
    if(button == SDL_BUTTON_LEFT) { return  Scancode::M_LEFT; }
    else if(button == SDL_BUTTON_RIGHT) { return Scancode::M_RIGHT; }
    else if(button == SDL_BUTTON_MIDDLE) { return Scancode::M_MIDDLE; }
    else if(button == SDL_BUTTON_X1 ) { return Scancode::M_SPECIAL_0; }
    else if(button == SDL_BUTTON_X2 ) { return Scancode::M_SPECIAL_1; }
    return Scancode::UNKNOWN;
}
    
Scancode scancodeFromSDLKeyScancode(SDL_Scancode key) {
    switch(key) {
        case SDL_SCANCODE_0                      : return Scancode::K_0;
        case SDL_SCANCODE_1                      : return Scancode::K_1;
        case SDL_SCANCODE_2                      : return Scancode::K_2;
        case SDL_SCANCODE_3                      : return Scancode::K_3;
        case SDL_SCANCODE_4                      : return Scancode::K_4;
        case SDL_SCANCODE_5                      : return Scancode::K_5;
        case SDL_SCANCODE_6                      : return Scancode::K_6;
        case SDL_SCANCODE_7                      : return Scancode::K_7;
        case SDL_SCANCODE_8                      : return Scancode::K_8;
        case SDL_SCANCODE_9                      : return Scancode::K_9;
        case SDL_SCANCODE_A                      : return Scancode::K_A;
        case SDL_SCANCODE_B                      : return Scancode::K_B;
        case SDL_SCANCODE_C                      : return Scancode::K_C;
        case SDL_SCANCODE_D                      : return Scancode::K_D;
        case SDL_SCANCODE_E                      : return Scancode::K_E;
        case SDL_SCANCODE_F                      : return Scancode::K_F;
        case SDL_SCANCODE_G                      : return Scancode::K_G;
        case SDL_SCANCODE_H                      : return Scancode::K_H;
        case SDL_SCANCODE_I                      : return Scancode::K_I;
        case SDL_SCANCODE_J                      : return Scancode::K_J;
        case SDL_SCANCODE_K                      : return Scancode::K_K;
        case SDL_SCANCODE_L                      : return Scancode::K_L;
        case SDL_SCANCODE_M                      : return Scancode::K_M;
        case SDL_SCANCODE_N                      : return Scancode::K_N;
        case SDL_SCANCODE_O                      : return Scancode::K_O;
        case SDL_SCANCODE_P                      : return Scancode::K_P;
        case SDL_SCANCODE_Q                      : return Scancode::K_Q;
        case SDL_SCANCODE_R                      : return Scancode::K_R;
        case SDL_SCANCODE_S                      : return Scancode::K_S;
        case SDL_SCANCODE_T                      : return Scancode::K_T;
        case SDL_SCANCODE_U                      : return Scancode::K_U;
        case SDL_SCANCODE_V                      : return Scancode::K_V;
        case SDL_SCANCODE_W                      : return Scancode::K_W;
        case SDL_SCANCODE_X                      : return Scancode::K_X;
        case SDL_SCANCODE_Y                      : return Scancode::K_Y;
        case SDL_SCANCODE_Z                      : return Scancode::K_Z;
        case SDL_SCANCODE_KP_0                   : return Scancode::K_NUMPAD_0;
        case SDL_SCANCODE_KP_1                   : return Scancode::K_NUMPAD_1;
        case SDL_SCANCODE_KP_2                   : return Scancode::K_NUMPAD_2;
        case SDL_SCANCODE_KP_3                   : return Scancode::K_NUMPAD_3;
        case SDL_SCANCODE_KP_4                   : return Scancode::K_NUMPAD_4;
        case SDL_SCANCODE_KP_5                   : return Scancode::K_NUMPAD_5;
        case SDL_SCANCODE_KP_6                   : return Scancode::K_NUMPAD_6;
        case SDL_SCANCODE_KP_7                   : return Scancode::K_NUMPAD_7;
        case SDL_SCANCODE_KP_8                   : return Scancode::K_NUMPAD_8;
        case SDL_SCANCODE_KP_9                   : return Scancode::K_NUMPAD_9;
        case SDL_SCANCODE_KP_00                  : return Scancode::K_NUMPAD_00;
        case SDL_SCANCODE_KP_000                 : return Scancode::K_NUMPAD_000;
        case SDL_SCANCODE_KP_A                   : return Scancode::K_NUMPAD_A;
        case SDL_SCANCODE_KP_B                   : return Scancode::K_NUMPAD_B;
        case SDL_SCANCODE_KP_C                   : return Scancode::K_NUMPAD_C;
        case SDL_SCANCODE_KP_D                   : return Scancode::K_NUMPAD_D;
        case SDL_SCANCODE_KP_E                   : return Scancode::K_NUMPAD_E;
        case SDL_SCANCODE_KP_F                   : return Scancode::K_NUMPAD_F;
        case SDL_SCANCODE_KP_AMPERSAND           : return Scancode::K_NUMPAD_AMPERSAND;
        case SDL_SCANCODE_KP_AT                  : return Scancode::K_NUMPAD_AT;
        case SDL_SCANCODE_KP_BACKSPACE           : return Scancode::K_NUMPAD_BACKSPACE;
        case SDL_SCANCODE_KP_BINARY              : return Scancode::K_NUMPAD_BINARY;
        case SDL_SCANCODE_KP_CLEAR               : return Scancode::K_NUMPAD_CLEAR;
        case SDL_SCANCODE_KP_CLEARENTRY          : return Scancode::K_NUMPAD_CLEAR_ENTRY;
        case SDL_SCANCODE_KP_COLON               : return Scancode::K_NUMPAD_COLON;
        case SDL_SCANCODE_KP_COMMA               : return Scancode::K_NUMPAD_COMMA;
        case SDL_SCANCODE_KP_DBLAMPERSAND        : return Scancode::K_NUMPAD_DOUBLE_AMPERSAND;
        case SDL_SCANCODE_KP_DBLVERTICALBAR      : return Scancode::K_NUMPAD_DOUBLE_VERTICAL;
        case SDL_SCANCODE_KP_DECIMAL             : return Scancode::K_NUMPAD_DECIMAL_POINT;
        case SDL_SCANCODE_KP_DIVIDE              : return Scancode::K_NUMPAD_DIVIDE;
        case SDL_SCANCODE_KP_ENTER               : return Scancode::K_NUMPAD_ENTER;
        case SDL_SCANCODE_KP_EQUALS              : return Scancode::K_NUMPAD_EQUALS;
        case SDL_SCANCODE_KP_EQUALSAS400         : return Scancode::K_NUMPAD_EQUALS_ALT;
        case SDL_SCANCODE_KP_EXCLAM              : return Scancode::K_NUMPAD_FACTORIAL;
        case SDL_SCANCODE_KP_GREATER             : return Scancode::K_NUMPAD_GREATER_THAN;
        case SDL_SCANCODE_KP_HASH                : return Scancode::K_NUMPAD_HASH;
        case SDL_SCANCODE_KP_HEXADECIMAL         : return Scancode::K_NUMPAD_HEX;
        case SDL_SCANCODE_KP_LEFTBRACE           : return Scancode::K_NUMPAD_BRACE_LEFT;
        case SDL_SCANCODE_KP_LEFTPAREN           : return Scancode::K_NUMPAD_PARENTHESIS_LEFT;
        case SDL_SCANCODE_KP_LESS                : return Scancode::K_NUMPAD_LESS_THAN;
        case SDL_SCANCODE_KP_MEMADD              : return Scancode::K_NUMPAD_MEMORY_ADD;
        case SDL_SCANCODE_KP_MEMCLEAR            : return Scancode::K_NUMPAD_MEMORY_CLEAR;
        case SDL_SCANCODE_KP_MEMDIVIDE           : return Scancode::K_NUMPAD_MEMORY_DIVIDE;
        case SDL_SCANCODE_KP_MEMMULTIPLY         : return Scancode::K_NUMPAD_MEMORY_MULTIPLY;
        case SDL_SCANCODE_KP_MEMRECALL           : return Scancode::K_NUMPAD_MEMORY_CALL;
        case SDL_SCANCODE_KP_MEMSTORE            : return Scancode::K_NUMPAD_MEMORY_STORE;
        case SDL_SCANCODE_KP_MEMSUBTRACT         : return Scancode::K_NUMPAD_MEMORY_SUBTRACT;
        case SDL_SCANCODE_KP_MINUS               : return Scancode::K_NUMPAD_SUBTRACT;
        case SDL_SCANCODE_KP_MULTIPLY            : return Scancode::K_NUMPAD_MULTIPLY;
        case SDL_SCANCODE_KP_OCTAL               : return Scancode::K_NUMPAD_OCT;
        case SDL_SCANCODE_KP_PERCENT             : return Scancode::K_NUMPAD_PERCENT;
        case SDL_SCANCODE_KP_PERIOD              : return Scancode::K_NUMPAD_PERIOD;
        case SDL_SCANCODE_KP_PLUS                : return Scancode::K_NUMPAD_ADD;
        case SDL_SCANCODE_KP_PLUSMINUS           : return Scancode::K_NUMPAD_PLUS_MINUS;
        case SDL_SCANCODE_KP_POWER               : return Scancode::K_NUMPAD_CARET;
        case SDL_SCANCODE_KP_RIGHTBRACE          : return Scancode::K_NUMPAD_BRACE_RIGHT;
        case SDL_SCANCODE_KP_RIGHTPAREN          : return Scancode::K_NUMPAD_PARENTHESIS_RIGHT;
        case SDL_SCANCODE_KP_SPACE               : return Scancode::K_NUMPAD_SPACE;
        case SDL_SCANCODE_KP_TAB                 : return Scancode::K_NUMPAD_TAB;
        case SDL_SCANCODE_KP_VERTICALBAR         : return Scancode::K_NUMPAD_VERTICAL;
        case SDL_SCANCODE_KP_XOR                 : return Scancode::K_NUMPAD_XOR;
        case SDL_SCANCODE_F1                     : return Scancode::K_F1;
        case SDL_SCANCODE_F2                     : return Scancode::K_F2;
        case SDL_SCANCODE_F3                     : return Scancode::K_F3;
        case SDL_SCANCODE_F4                     : return Scancode::K_F4;
        case SDL_SCANCODE_F5                     : return Scancode::K_F5;
        case SDL_SCANCODE_F6                     : return Scancode::K_F6;
        case SDL_SCANCODE_F7                     : return Scancode::K_F7;
        case SDL_SCANCODE_F8                     : return Scancode::K_F8;
        case SDL_SCANCODE_F9                     : return Scancode::K_F9;
        case SDL_SCANCODE_F10                    : return Scancode::K_F10;
        case SDL_SCANCODE_F11                    : return Scancode::K_F11;
        case SDL_SCANCODE_F12                    : return Scancode::K_F12;
        case SDL_SCANCODE_F13                    : return Scancode::K_F13;
        case SDL_SCANCODE_F14                    : return Scancode::K_F14;
        case SDL_SCANCODE_F15                    : return Scancode::K_F15;
        case SDL_SCANCODE_F16                    : return Scancode::K_F16;
        case SDL_SCANCODE_F17                    : return Scancode::K_F17;
        case SDL_SCANCODE_F18                    : return Scancode::K_F18;
        case SDL_SCANCODE_F19                    : return Scancode::K_F19;
        case SDL_SCANCODE_F20                    : return Scancode::K_F20;
        case SDL_SCANCODE_F21                    : return Scancode::K_F21;
        case SDL_SCANCODE_F22                    : return Scancode::K_F22;
        case SDL_SCANCODE_F23                    : return Scancode::K_F23;
        case SDL_SCANCODE_F24                    : return Scancode::K_F24;
        case SDL_SCANCODE_AC_BACK                : return Scancode::K_APP_BACK;
        case SDL_SCANCODE_AC_BOOKMARKS           : return Scancode::K_APP_FAVORITES;
        case SDL_SCANCODE_AC_FORWARD             : return Scancode::K_APP_FORWARD;
        case SDL_SCANCODE_AC_HOME                : return Scancode::K_APP_HOME;
        case SDL_SCANCODE_AC_REFRESH             : return Scancode::K_APP_REFRESH;
        case SDL_SCANCODE_AC_SEARCH              : return Scancode::K_APP_SEARCH;
        case SDL_SCANCODE_AC_STOP                : return Scancode::K_APP_STOP;
        case SDL_SCANCODE_AGAIN                  : return Scancode::K_REDO;
        case SDL_SCANCODE_ALTERASE               : return Scancode::K_ERASE;
        case SDL_SCANCODE_APOSTROPHE             : return Scancode::K_APOSTROPHE;
        case SDL_SCANCODE_APPLICATION            : return Scancode::K_APP_MENU;
        case SDL_SCANCODE_AUDIOMUTE              : return Scancode::K_SND_MUTE;
        case SDL_SCANCODE_AUDIONEXT              : return Scancode::K_SND_NEXT;
        case SDL_SCANCODE_AUDIOPLAY              : return Scancode::K_SND_PLAY;
        case SDL_SCANCODE_AUDIOPREV              : return Scancode::K_SND_PREV;
        case SDL_SCANCODE_AUDIOSTOP              : return Scancode::K_SND_STOP;
        case SDL_SCANCODE_BACKSLASH              : return Scancode::K_BACKSLASH;
        case SDL_SCANCODE_BACKSPACE              : return Scancode::K_BACKSPACE;
        case SDL_SCANCODE_BRIGHTNESSDOWN         : return Scancode::K_BRIGHTNESS_DOWN;
        case SDL_SCANCODE_BRIGHTNESSUP           : return Scancode::K_BRIGHTNESS_UP;
        case SDL_SCANCODE_CALCULATOR             : return Scancode::K_CALCULATOR;
        case SDL_SCANCODE_CANCEL                 : return Scancode::K_CANCEL;
        case SDL_SCANCODE_CAPSLOCK               : return Scancode::K_CAPS_LOCK;
        case SDL_SCANCODE_CLEAR                  : return Scancode::K_CLEAR;
        case SDL_SCANCODE_CLEARAGAIN             : return Scancode::K_CLEAR_AGAIN;
        case SDL_SCANCODE_COMMA                  : return Scancode::K_COMMA;
        case SDL_SCANCODE_COMPUTER               : return Scancode::K_MY_COMPUTER;
        case SDL_SCANCODE_COPY                   : return Scancode::K_COPY;
        case SDL_SCANCODE_CRSEL                  : return Scancode::K_CR_CEL;
        case SDL_SCANCODE_CURRENCYSUBUNIT        : return Scancode::K_CURRENCY_SUB_UNIT;
        case SDL_SCANCODE_CURRENCYUNIT           : return Scancode::K_CURRENCY_UNIT;
        case SDL_SCANCODE_CUT                    : return Scancode::K_CUT;
        case SDL_SCANCODE_DECIMALSEPARATOR       : return Scancode::K_DECIMAL_POINT;
        case SDL_SCANCODE_DELETE                 : return Scancode::K_DELETE;
        case SDL_SCANCODE_DISPLAYSWITCH          : return Scancode::K_VIDEO_MODE_SWITCH;
        case SDL_SCANCODE_DOWN                   : return Scancode::K_ARROW_DOWN;
        case SDL_SCANCODE_EJECT                  : return Scancode::K_EJECT;
        case SDL_SCANCODE_END                    : return Scancode::K_END;
        case SDL_SCANCODE_EQUALS                 : return Scancode::K_EQUALS;
        case SDL_SCANCODE_ESCAPE                 : return Scancode::K_ESCAPE;
        case SDL_SCANCODE_EXECUTE                : return Scancode::K_EXECUTE;
        case SDL_SCANCODE_EXSEL                  : return Scancode::K_EX_SEL;
        case SDL_SCANCODE_FIND                   : return Scancode::K_FIND;
        case SDL_SCANCODE_GRAVE                  : return Scancode::K_GRAVE;
        case SDL_SCANCODE_HELP                   : return Scancode::K_HELP;
        case SDL_SCANCODE_HOME                   : return Scancode::K_HOME;
        case SDL_SCANCODE_INSERT                 : return Scancode::K_INSERT;
        case SDL_SCANCODE_INTERNATIONAL1         : return Scancode::K_INTERNATIONAL_1;
        case SDL_SCANCODE_INTERNATIONAL2         : return Scancode::K_INTERNATIONAL_2;
        case SDL_SCANCODE_INTERNATIONAL3         : return Scancode::K_INTERNATIONAL_3;
        case SDL_SCANCODE_INTERNATIONAL4         : return Scancode::K_INTERNATIONAL_4;
        case SDL_SCANCODE_INTERNATIONAL5         : return Scancode::K_INTERNATIONAL_5;
        case SDL_SCANCODE_INTERNATIONAL6         : return Scancode::K_INTERNATIONAL_6;
        case SDL_SCANCODE_INTERNATIONAL7         : return Scancode::K_INTERNATIONAL_7;
        case SDL_SCANCODE_INTERNATIONAL8         : return Scancode::K_INTERNATIONAL_8;
        case SDL_SCANCODE_INTERNATIONAL9         : return Scancode::K_INTERNATIONAL_9;
        case SDL_SCANCODE_KBDILLUMDOWN           : return Scancode::K_ILLUMINATION_DOWN;
        case SDL_SCANCODE_KBDILLUMTOGGLE         : return Scancode::K_ILLUMINATION_TOGGLE;
        case SDL_SCANCODE_KBDILLUMUP             : return Scancode::K_ILLUMINATION_UP;
        case SDL_SCANCODE_LALT                   : return Scancode::K_ALT_LEFT;
        case SDL_SCANCODE_LANG1                  : return Scancode::K_LANG_1;
        case SDL_SCANCODE_LANG2                  : return Scancode::K_LANG_2;
        case SDL_SCANCODE_LANG3                  : return Scancode::K_LANG_3;
        case SDL_SCANCODE_LANG4                  : return Scancode::K_LANG_4;
        case SDL_SCANCODE_LANG5                  : return Scancode::K_LANG_5;
        case SDL_SCANCODE_LANG6                  : return Scancode::K_LANG_6;
        case SDL_SCANCODE_LANG7                  : return Scancode::K_LANG_7;
        case SDL_SCANCODE_LANG8                  : return Scancode::K_LANG_8;
        case SDL_SCANCODE_LANG9                  : return Scancode::K_LANG_9;
        case SDL_SCANCODE_LCTRL                  : return Scancode::K_CONTROL_LEFT;
        case SDL_SCANCODE_LEFT                   : return Scancode::K_ARROW_LEFT;
        case SDL_SCANCODE_LEFTBRACKET            : return Scancode::K_BRACKET_LEFT;
        case SDL_SCANCODE_LGUI                   : return Scancode::K_GUI_LEFT;
        case SDL_SCANCODE_LSHIFT                 : return Scancode::K_SHIFT_LEFT;
        case SDL_SCANCODE_MAIL                   : return Scancode::K_MAIL;
        case SDL_SCANCODE_MEDIASELECT            : return Scancode::K_MEDIA_SELECT;
        case SDL_SCANCODE_MENU                   : return Scancode::K_MENU;
        case SDL_SCANCODE_MINUS                  : return Scancode::K_SUBTRACT;
        case SDL_SCANCODE_MODE                   : return Scancode::K_MODE_SWITCH;
        case SDL_SCANCODE_MUTE                   : return Scancode::K_MUTE;
        case SDL_SCANCODE_NONUSBACKSLASH         : return Scancode::UNKNOWN;
        case SDL_SCANCODE_NONUSHASH              : return Scancode::UNKNOWN;
        case SDL_SCANCODE_NUMLOCKCLEAR           : return Scancode::K_NUMLOCK;
        case SDL_SCANCODE_OPER                   : return Scancode::K_OPER;
        case SDL_SCANCODE_OUT                    : return Scancode::K_OUT;
        case SDL_SCANCODE_PAGEDOWN               : return Scancode::K_PAGE_DOWN;
        case SDL_SCANCODE_PAGEUP                 : return Scancode::K_PAGE_UP;
        case SDL_SCANCODE_PASTE                  : return Scancode::K_PASTE;
        case SDL_SCANCODE_PAUSE                  : return Scancode::K_PAUSE;
        case SDL_SCANCODE_PERIOD                 : return Scancode::K_PERIOD;
        case SDL_SCANCODE_POWER                  : return Scancode::K_POWER;
        case SDL_SCANCODE_PRINTSCREEN            : return Scancode::K_PRINTSCREEN;
        case SDL_SCANCODE_PRIOR                  : return Scancode::K_PRIOR;
        case SDL_SCANCODE_RALT                   : return Scancode::K_ALT_RIGHT;
        case SDL_SCANCODE_RCTRL                  : return Scancode::K_CONTROL_RIGHT;
        case SDL_SCANCODE_RETURN                 : return Scancode::K_RETURN;
        case SDL_SCANCODE_RETURN2                : return Scancode::K_RETURN_ALT;
        case SDL_SCANCODE_RGUI                   : return Scancode::K_GUI_RIGHT;
        case SDL_SCANCODE_RIGHT                  : return Scancode::K_ARROW_RIGHT;
        case SDL_SCANCODE_RIGHTBRACKET           : return Scancode::K_BRACKET_RIGHT;
        case SDL_SCANCODE_RSHIFT                 : return Scancode::K_SHIFT_RIGHT;
        case SDL_SCANCODE_SCROLLLOCK             : return Scancode::K_SCROLL_LOCK;
        case SDL_SCANCODE_SELECT                 : return Scancode::K_SELECT;
        case SDL_SCANCODE_SEMICOLON              : return Scancode::K_SEMICOLON;
        case SDL_SCANCODE_SEPARATOR              : return Scancode::K_SEPARATOR;
        case SDL_SCANCODE_SLASH                  : return Scancode::K_SLASH;
        case SDL_SCANCODE_SLEEP                  : return Scancode::K_SLEEP;
        case SDL_SCANCODE_SPACE                  : return Scancode::K_SPACEBAR;
        case SDL_SCANCODE_STOP                   : return Scancode::K_STOP;
        case SDL_SCANCODE_SYSREQ                 : return Scancode::K_SYS_REQ;
        case SDL_SCANCODE_TAB                    : return Scancode::K_TAB;
        case SDL_SCANCODE_THOUSANDSSEPARATOR     : return Scancode::K_THOUSANDS_SEPARATOR;
        case SDL_SCANCODE_UNDO                   : return Scancode::K_UNDO;
        case SDL_SCANCODE_UP                     : return Scancode::K_ARROW_UP;
        case SDL_SCANCODE_VOLUMEDOWN             : return Scancode::K_SND_VOLUME_DOWN;
        case SDL_SCANCODE_VOLUMEUP               : return Scancode::K_SND_VOLUME_UP;
        case SDL_SCANCODE_WWW                    : return Scancode::K_WORLD_WIDE_WEB;
        default : return Scancode::UNKNOWN;
    }
}


}

InputState::InputState() {
    for(uint32_t i = 0; i < Input::Scancode::ENUM_SIZE; ++ i) {
        mPressed[i] = false;
    }
}

InputState::~InputState() { }

bool InputState::isPressed(Input::Scancode button) const {
    assert(button < Input::Scancode::ENUM_SIZE && "Button scancode out of array bounds in ScancodeState::isPressed()");
    if(button == Input::Scancode::UNKNOWN) {
        return false;
    }
    return mPressed[button];
}

int32_t InputState::getMouseX() const {
    return mMouseX;
}
int32_t InputState::getMouseY() const {
    return mMouseY;
}
int32_t InputState::getMouseDX() const {
    return mMouseDX;
}
int32_t InputState::getMouseDY() const {
    return mMouseDY;
}
void InputState::setMouseDelta(int32_t dx, int32_t dy) {
    mMouseDX = dx;
    mMouseDY = dy;
}

void InputState::updateKeysFromSDL() {
    int sdlKeystateSize;
    const Uint8* keystates = SDL_GetKeyboardState(&sdlKeystateSize);
    for(int i = 0; i < sdlKeystateSize; ++ i) {
        mPressed[Input::scancodeFromSDLKeyScancode(static_cast<SDL_Scancode>(i))] = keystates[i];
    }
}

void InputState::updateMouseFromSDL() {
    int x;
    int y;
    Uint32 buttonBitmask = SDL_GetMouseState(&x, &y);
    
    mMouseX = x;
    mMouseY = y;

    mPressed[Input::Scancode::M_LEFT] = (buttonBitmask & SDL_BUTTON(SDL_BUTTON_LEFT)) > 0 ? true : false;
    mPressed[Input::Scancode::M_RIGHT] = (buttonBitmask & SDL_BUTTON(SDL_BUTTON_RIGHT)) > 0 ? true : false;
    mPressed[Input::Scancode::M_MIDDLE] = (buttonBitmask & SDL_BUTTON(SDL_BUTTON_MIDDLE)) > 0 ? true : false;
    mPressed[Input::Scancode::M_SPECIAL_0] = (buttonBitmask & SDL_BUTTON(SDL_BUTTON_X1)) > 0 ? true : false;
    mPressed[Input::Scancode::M_SPECIAL_1] = (buttonBitmask & SDL_BUTTON(SDL_BUTTON_X2)) > 0 ? true : false;
}

void InputState::setState(Input::Scancode button, bool pressed) {
    assert(button < Input::Scancode::ENUM_SIZE && "Button scancode out of array bounds in ScancodeState::setState()");
    if(button != Input::Scancode::UNKNOWN) {
        mPressed[button] = pressed;
    }
}

}
