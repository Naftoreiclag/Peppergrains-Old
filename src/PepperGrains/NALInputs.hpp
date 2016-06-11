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

enum MouseButton {
    LEFT,
    MIDDLE,
    RIGHT,
    SPECIAL_A,
    SPECIAL_B
};

enum KeyScancode {
    UNKNOWN,
    NUM_0, NUM_1, NUM_2, NUM_3, NUM_4, 
    NUM_5, NUM_6, NUM_7, NUM_8, NUM_9,
    A, B, C, D, E, F, G, H, I, J, K, L, M,
    N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
    NUMPAD_0, NUMPAD_1, NUMPAD_2, NUMPAD_3, NUMPAD_4, 
    NUMPAD_5, NUMPAD_6, NUMPAD_7, NUMPAD_8, NUMPAD_9,
    NUMPAD_COMMA, NUMPAD_ENTER, NUM_EQUALS,
    F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, F13, F14, F15,
    ADD,
    ALT_LEFT,
    ALT_RIGHT,
    APOSTROPHE,
    APP_BACK,
    APP_FAVORITES,
    APP_FORWARD,
    APP_HOME,
    APP_MENU,
    APP_REFRESH,
    APP_SEARCH,
    APP_STOP,
    ARROW_DOWN,
    ARROW_LEFT,
    ARROW_RIGHT,
    ARROW_UP,
    BACKSLASH,
    BACKSPACE,
    BRACKET_LEFT,
    BRACKET_RIGHT,
    BRIGHTNESS_DOWN,
    BRIGHTNESS_UP,
    CALCULATOR,
    CAPS_LOCK,
    COLON,
    COMMA,
    CONTROL_LEFT,
    CONTROL_RIGHT,
    DECIMAL,
    DELETE,
    DIVIDE,
    END,
    EQUALS,
    ESCAPE,
    GRAVE,
    GUI_LEFT,
    GUI_RIGHT,
    HOME,
    INSERT,
    MAIL,
    MEDIA_SELECT,
    MULTIPLY,
    MY_COMPUTER,
    PAGE_DOWN,
    PAGE_UP,
    PAUSE,
    PERIOD,
    POWER,
    RETURN,
    SCROLL_LOCK,
    SEMICOLON,
    SHIFT_LEFT,
    SHIFT_RIGHT,
    SLASH,
    SLEEP,
    SND_MUTE,
    SND_NEXT,
    SND_PLAY,
    SND_PREV,
    SND_STOP,
    SND_VOLUME_DOWN,
    SND_VOLUME_UP,
    SPACEBAR,
    STOP,
    SUBTRACT,
    SYS_REQ,
    TAB,

    ENUM_SIZE
};

KeyScancode keyScancodeFromSDL(SDL_Scancode key) {
    switch(key) {
        case SDL_SCANCODE_0                      : return KeyScancode::NUM_0;
        case SDL_SCANCODE_1                      : return KeyScancode::NUM_1;
        case SDL_SCANCODE_2                      : return KeyScancode::NUM_2;
        case SDL_SCANCODE_3                      : return KeyScancode::NUM_3;
        case SDL_SCANCODE_4                      : return KeyScancode::NUM_4;
        case SDL_SCANCODE_5                      : return KeyScancode::NUM_5;
        case SDL_SCANCODE_6                      : return KeyScancode::NUM_6;
        case SDL_SCANCODE_7                      : return KeyScancode::NUM_7;
        case SDL_SCANCODE_8                      : return KeyScancode::NUM_8;
        case SDL_SCANCODE_9                      : return KeyScancode::NUM_9;
        case SDL_SCANCODE_A                      : return KeyScancode::A;
        case SDL_SCANCODE_B                      : return KeyScancode::B;
        case SDL_SCANCODE_C                      : return KeyScancode::C;
        case SDL_SCANCODE_D                      : return KeyScancode::D;
        case SDL_SCANCODE_E                      : return KeyScancode::E;
        case SDL_SCANCODE_F                      : return KeyScancode::F;
        case SDL_SCANCODE_G                      : return KeyScancode::G;
        case SDL_SCANCODE_H                      : return KeyScancode::H;
        case SDL_SCANCODE_I                      : return KeyScancode::I;
        case SDL_SCANCODE_J                      : return KeyScancode::J;
        case SDL_SCANCODE_K                      : return KeyScancode::K;
        case SDL_SCANCODE_L                      : return KeyScancode::L;
        case SDL_SCANCODE_M                      : return KeyScancode::M;
        case SDL_SCANCODE_N                      : return KeyScancode::N;
        case SDL_SCANCODE_O                      : return KeyScancode::O;
        case SDL_SCANCODE_P                      : return KeyScancode::P;
        case SDL_SCANCODE_Q                      : return KeyScancode::Q;
        case SDL_SCANCODE_R                      : return KeyScancode::R;
        case SDL_SCANCODE_S                      : return KeyScancode::S;
        case SDL_SCANCODE_T                      : return KeyScancode::T;
        case SDL_SCANCODE_U                      : return KeyScancode::U;
        case SDL_SCANCODE_V                      : return KeyScancode::V;
        case SDL_SCANCODE_W                      : return KeyScancode::W;
        case SDL_SCANCODE_X                      : return KeyScancode::X;
        case SDL_SCANCODE_Y                      : return KeyScancode::Y;
        case SDL_SCANCODE_Z                      : return KeyScancode::Z;
        case SDL_SCANCODE_KP_0                   : return KeyScancode::NUMPAD_0;
        case SDL_SCANCODE_KP_1                   : return KeyScancode::NUMPAD_1;
        case SDL_SCANCODE_KP_2                   : return KeyScancode::NUMPAD_2;
        case SDL_SCANCODE_KP_3                   : return KeyScancode::NUMPAD_3;
        case SDL_SCANCODE_KP_4                   : return KeyScancode::NUMPAD_4;
        case SDL_SCANCODE_KP_5                   : return KeyScancode::NUMPAD_5;
        case SDL_SCANCODE_KP_6                   : return KeyScancode::NUMPAD_6;
        case SDL_SCANCODE_KP_7                   : return KeyScancode::NUMPAD_7;
        case SDL_SCANCODE_KP_8                   : return KeyScancode::NUMPAD_8;
        case SDL_SCANCODE_KP_9                   : return KeyScancode::NUMPAD_9;
        case SDL_SCANCODE_F1                     : return KeyScancode::F1;
        case SDL_SCANCODE_F2                     : return KeyScancode::F2;
        case SDL_SCANCODE_F3                     : return KeyScancode::F3;
        case SDL_SCANCODE_F4                     : return KeyScancode::F4;
        case SDL_SCANCODE_F5                     : return KeyScancode::F5;
        case SDL_SCANCODE_F6                     : return KeyScancode::F6;
        case SDL_SCANCODE_F7                     : return KeyScancode::F7;
        case SDL_SCANCODE_F8                     : return KeyScancode::F8;
        case SDL_SCANCODE_F9                     : return KeyScancode::F9;
        case SDL_SCANCODE_F10                    : return KeyScancode::F10;
        case SDL_SCANCODE_F11                    : return KeyScancode::F11;
        case SDL_SCANCODE_F12                    : return KeyScancode::F12;
        case SDL_SCANCODE_F13                    : return KeyScancode::F13;
        case SDL_SCANCODE_F14                    : return KeyScancode::F14;
        case SDL_SCANCODE_F15                    : return KeyScancode::F15;
        case SDL_SCANCODE_F16                    : return KeyScancode::UNKNOWN;
        case SDL_SCANCODE_F17                    : return KeyScancode::UNKNOWN;
        case SDL_SCANCODE_F18                    : return KeyScancode::UNKNOWN;
        case SDL_SCANCODE_F19                    : return KeyScancode::UNKNOWN;
        case SDL_SCANCODE_F20                    : return KeyScancode::UNKNOWN;
        case SDL_SCANCODE_F21                    : return KeyScancode::UNKNOWN;
        case SDL_SCANCODE_F22                    : return KeyScancode::UNKNOWN;
        case SDL_SCANCODE_F23                    : return KeyScancode::UNKNOWN;
        case SDL_SCANCODE_F24                    : return KeyScancode::UNKNOWN;
        case SDL_SCANCODE_AC_BACK                : return KeyScancode::APP_BACK;
        case SDL_SCANCODE_AC_BOOKMARKS           : return KeyScancode::APP_FAVORITES;
        case SDL_SCANCODE_AC_FORWARD             : return KeyScancode::APP_FORWARD;
        case SDL_SCANCODE_AC_HOME                : return KeyScancode::APP_HOME;
        case SDL_SCANCODE_AC_REFRESH             : return KeyScancode::APP_REFRESH;
        case SDL_SCANCODE_AC_SEARCH              : return KeyScancode::APP_SEARCH;
        case SDL_SCANCODE_AC_STOP                : return KeyScancode::APP_STOP;
        case SDL_SCANCODE_AGAIN                  : return KeyScancode::UNKNOWN;
        case SDL_SCANCODE_ALTERASE               : return KeyScancode::UNKNOWN;
        case SDL_SCANCODE_APOSTROPHE             : return KeyScancode::APOSTROPHE;
        case SDL_SCANCODE_APPLICATION            : return KeyScancode::APP_MENU;
        case SDL_SCANCODE_AUDIOMUTE              : return KeyScancode::SND_MUTE;
        case SDL_SCANCODE_AUDIONEXT              : return KeyScancode::SND_NEXT;
        case SDL_SCANCODE_AUDIOPLAY              : return KeyScancode::SND_PLAY;
        case SDL_SCANCODE_AUDIOPREV              : return KeyScancode::SND_PREV;
        case SDL_SCANCODE_AUDIOSTOP              : return KeyScancode::SND_STOP;
        case SDL_SCANCODE_BACKSLASH              : return KeyScancode::BACKSLASH;
        case SDL_SCANCODE_BACKSPACE              : return KeyScancode::BACKSPACE;
        case SDL_SCANCODE_BRIGHTNESSDOWN         : return KeyScancode::UNKNOWN;
        case SDL_SCANCODE_BRIGHTNESSUP           : return KeyScancode::UNKNOWN;
        case SDL_SCANCODE_CALCULATOR             : return KeyScancode::CALCULATOR;
        case SDL_SCANCODE_CANCEL                 : return KeyScancode::UNKNOWN;
        case SDL_SCANCODE_CAPSLOCK               : return KeyScancode::CAPS_LOCK;
        case SDL_SCANCODE_CLEAR                  : return KeyScancode::UNKNOWN;
        case SDL_SCANCODE_CLEARAGAIN             : return KeyScancode::UNKNOWN;
        case SDL_SCANCODE_COMMA                  : return KeyScancode::COMMA;
        case SDL_SCANCODE_COMPUTER               : return KeyScancode::MY_COMPUTER;
        case SDL_SCANCODE_COPY                   : return KeyScancode::UNKNOWN;
        case SDL_SCANCODE_CRSEL                  : return KeyScancode::UNKNOWN;
        case SDL_SCANCODE_CURRENCYSUBUNIT        : return KeyScancode::UNKNOWN;
        case SDL_SCANCODE_CURRENCYUNIT           : return KeyScancode::UNKNOWN;
        case SDL_SCANCODE_CUT                    : return KeyScancode::UNKNOWN;
        case SDL_SCANCODE_DECIMALSEPARATOR       : return KeyScancode::UNKNOWN;
        case SDL_SCANCODE_DELETE                 : return KeyScancode::DELETE;
        case SDL_SCANCODE_DISPLAYSWITCH          : return KeyScancode::UNKNOWN;
        case SDL_SCANCODE_DOWN                   : return KeyScancode::ARROW_DOWN;
        case SDL_SCANCODE_EJECT                  : return KeyScancode::UNKNOWN;
        case SDL_SCANCODE_END                    : return KeyScancode::END;
        case SDL_SCANCODE_EQUALS                 : return KeyScancode::EQUALS;
        case SDL_SCANCODE_ESCAPE                 : return KeyScancode::ESCAPE;
        case SDL_SCANCODE_EXECUTE                : return KeyScancode::UNKNOWN;
        case SDL_SCANCODE_EXSEL                  : return KeyScancode::UNKNOWN;
        case SDL_SCANCODE_FIND                   : return KeyScancode::UNKNOWN;
        case SDL_SCANCODE_GRAVE                  : return KeyScancode::GRAVE;
        case SDL_SCANCODE_HELP                   : return KeyScancode::UNKNOWN;
        case SDL_SCANCODE_HOME                   : return KeyScancode::HOME;
        case SDL_SCANCODE_INSERT                 : return KeyScancode::INSERT;
        case SDL_SCANCODE_INTERNATIONAL1         : return KeyScancode::UNKNOWN;
        case SDL_SCANCODE_INTERNATIONAL2         : return KeyScancode::UNKNOWN;
        case SDL_SCANCODE_INTERNATIONAL3         : return KeyScancode::UNKNOWN;
        case SDL_SCANCODE_INTERNATIONAL4         : return KeyScancode::UNKNOWN;
        case SDL_SCANCODE_INTERNATIONAL5         : return KeyScancode::UNKNOWN;
        case SDL_SCANCODE_INTERNATIONAL6         : return KeyScancode::UNKNOWN;
        case SDL_SCANCODE_INTERNATIONAL7         : return KeyScancode::UNKNOWN;
        case SDL_SCANCODE_INTERNATIONAL8         : return KeyScancode::UNKNOWN;
        case SDL_SCANCODE_INTERNATIONAL9         : return KeyScancode::UNKNOWN;
        case SDL_SCANCODE_KBDILLUMDOWN           : return KeyScancode::UNKNOWN;
        case SDL_SCANCODE_KBDILLUMTOGGLE         : return KeyScancode::UNKNOWN;
        case SDL_SCANCODE_KBDILLUMUP             : return KeyScancode::UNKNOWN;
        case SDL_SCANCODE_KP_00                  : return KeyScancode::UNKNOWN;
        case SDL_SCANCODE_KP_000                 : return KeyScancode::UNKNOWN;
        case SDL_SCANCODE_KP_A                   : return KeyScancode::UNKNOWN;
        case SDL_SCANCODE_KP_AMPERSAND           : return KeyScancode::UNKNOWN;
        case SDL_SCANCODE_KP_AT                  : return KeyScancode::UNKNOWN;
        case SDL_SCANCODE_KP_B                   : return KeyScancode::UNKNOWN;
        case SDL_SCANCODE_KP_BACKSPACE           : return KeyScancode::UNKNOWN;
        case SDL_SCANCODE_KP_BINARY              : return KeyScancode::UNKNOWN;
        case SDL_SCANCODE_KP_C                   : return KeyScancode::UNKNOWN;
        case SDL_SCANCODE_KP_CLEAR               : return KeyScancode::UNKNOWN;
        case SDL_SCANCODE_KP_CLEARENTRY          : return KeyScancode::UNKNOWN;
        case SDL_SCANCODE_KP_COLON               : return KeyScancode::COLON;
        case SDL_SCANCODE_KP_COMMA               : return KeyScancode::NUMPAD_COMMA;
        case SDL_SCANCODE_KP_D                   : return KeyScancode::UNKNOWN;
        case SDL_SCANCODE_KP_DBLAMPERSAND        : return KeyScancode::UNKNOWN;
        case SDL_SCANCODE_KP_DBLVERTICALBAR      : return KeyScancode::UNKNOWN;
        case SDL_SCANCODE_KP_DECIMAL             : return KeyScancode::DECIMAL;
        case SDL_SCANCODE_KP_DIVIDE              : return KeyScancode::DIVIDE;
        case SDL_SCANCODE_KP_E                   : return KeyScancode::UNKNOWN;
        case SDL_SCANCODE_KP_ENTER               : return KeyScancode::NUMPAD_ENTER;
        case SDL_SCANCODE_KP_EQUALS              : return KeyScancode::NUMPAD_EQUALS;
        case SDL_SCANCODE_KP_EQUALSAS400         : return KeyScancode::NUMPAD_EQUALS;
        case SDL_SCANCODE_KP_EXCLAM              : return KeyScancode::UNKNOWN;
        case SDL_SCANCODE_KP_F                   : return KeyScancode::UNKNOWN;
        case SDL_SCANCODE_KP_GREATER             : return KeyScancode::UNKNOWN;
        case SDL_SCANCODE_KP_HASH                : return KeyScancode::UNKNOWN;
        case SDL_SCANCODE_KP_HEXADECIMAL         : return KeyScancode::UNKNOWN;
        case SDL_SCANCODE_KP_LEFTBRACE           : return KeyScancode::UNKNOWN;
        case SDL_SCANCODE_KP_LEFTPAREN           : return KeyScancode::UNKNOWN;
        case SDL_SCANCODE_KP_LESS                : return KeyScancode::UNKNOWN;
        case SDL_SCANCODE_KP_MEMADD              : return KeyScancode::UNKNOWN;
        case SDL_SCANCODE_KP_MEMCLEAR            : return KeyScancode::UNKNOWN;
        case SDL_SCANCODE_KP_MEMDIVIDE           : return KeyScancode::UNKNOWN;
        case SDL_SCANCODE_KP_MEMMULTIPLY         : return KeyScancode::UNKNOWN;
        case SDL_SCANCODE_KP_MEMRECALL           : return KeyScancode::UNKNOWN;
        case SDL_SCANCODE_KP_MEMSTORE            : return KeyScancode::UNKNOWN;
        case SDL_SCANCODE_KP_MEMSUBTRACT         : return KeyScancode::UNKNOWN;
        case SDL_SCANCODE_KP_MINUS               : return KeyScancode::SUBTRACT;
        case SDL_SCANCODE_KP_MULTIPLY            : return KeyScancode::MULTIPLY;
        case SDL_SCANCODE_KP_OCTAL               : return KeyScancode::UNKNOWN;
        case SDL_SCANCODE_KP_PERCENT             : return KeyScancode::UNKNOWN;
        case SDL_SCANCODE_KP_PERIOD              : return KeyScancode::PERIOD;
        case SDL_SCANCODE_KP_PLUS                : return KeyScancode::ADD;
        case SDL_SCANCODE_KP_PLUSMINUS           : return KeyScancode::UNKNOWN;
        case SDL_SCANCODE_KP_POWER               : return KeyScancode::POWER;
        case SDL_SCANCODE_KP_RIGHTBRACE          : return KeyScancode::UNKNOWN;
        case SDL_SCANCODE_KP_RIGHTPAREN          : return KeyScancode::UNKNOWN;
        case SDL_SCANCODE_KP_SPACE               : return KeyScancode::UNKNOWN;
        case SDL_SCANCODE_KP_TAB                 : return KeyScancode::UNKNOWN;
        case SDL_SCANCODE_KP_VERTICALBAR         : return KeyScancode::UNKNOWN;
        case SDL_SCANCODE_KP_XOR                 : return KeyScancode::UNKNOWN;
        case SDL_SCANCODE_LALT                   : return KeyScancode::ALT_LEFT;
        case SDL_SCANCODE_LANG1                  : return KeyScancode::UNKNOWN;
        case SDL_SCANCODE_LANG2                  : return KeyScancode::UNKNOWN;
        case SDL_SCANCODE_LANG3                  : return KeyScancode::UNKNOWN;
        case SDL_SCANCODE_LANG4                  : return KeyScancode::UNKNOWN;
        case SDL_SCANCODE_LANG5                  : return KeyScancode::UNKNOWN;
        case SDL_SCANCODE_LANG6                  : return KeyScancode::UNKNOWN;
        case SDL_SCANCODE_LANG7                  : return KeyScancode::UNKNOWN;
        case SDL_SCANCODE_LANG8                  : return KeyScancode::UNKNOWN;
        case SDL_SCANCODE_LANG9                  : return KeyScancode::UNKNOWN;
        case SDL_SCANCODE_LCTRL                  : return KeyScancode::CONTROL_LEFT;
        case SDL_SCANCODE_LEFT                   : return KeyScancode::ARROW_LEFT;
        case SDL_SCANCODE_LEFTBRACKET            : return KeyScancode::BRACKET_LEFT;
        case SDL_SCANCODE_LGUI                   : return KeyScancode::GUI_LEFT;
        case SDL_SCANCODE_LSHIFT                 : return KeyScancode::SHIFT_LEFT;
        case SDL_SCANCODE_MAIL                   : return KeyScancode::MAIL;
        case SDL_SCANCODE_MEDIASELECT            : return KeyScancode::MEDIA_SELECT;
        case SDL_SCANCODE_MENU                   : return KeyScancode::UNKNOWN;
        case SDL_SCANCODE_MINUS                  : return KeyScancode::SUBTRACT;
        case SDL_SCANCODE_MODE                   : return KeyScancode::UNKNOWN;
        case SDL_SCANCODE_MUTE                   : return KeyScancode::UNKNOWN;
        case SDL_SCANCODE_NONUSBACKSLASH         : return KeyScancode::UNKNOWN;
        case SDL_SCANCODE_NONUSHASH              : return KeyScancode::UNKNOWN;
        case SDL_SCANCODE_NUMLOCKCLEAR           : return KeyScancode::UNKNOWN;
        case SDL_SCANCODE_OPER                   : return KeyScancode::UNKNOWN;
        case SDL_SCANCODE_OUT                    : return KeyScancode::UNKNOWN;
        case SDL_SCANCODE_PAGEDOWN               : return KeyScancode::PAGE_DOWN;
        case SDL_SCANCODE_PAGEUP                 : return KeyScancode::PAGE_UP;
        case SDL_SCANCODE_PASTE                  : return KeyScancode::UNKNOWN;
        case SDL_SCANCODE_PAUSE                  : return KeyScancode::PAUSE;
        case SDL_SCANCODE_PERIOD                 : return KeyScancode::UNKNOWN;
        case SDL_SCANCODE_POWER                  : return KeyScancode::UNKNOWN;
        case SDL_SCANCODE_PRINTSCREEN            : return KeyScancode::UNKNOWN;
        case SDL_SCANCODE_PRIOR                  : return KeyScancode::UNKNOWN;
        case SDL_SCANCODE_RALT                   : return KeyScancode::ALT_RIGHT;
        case SDL_SCANCODE_RCTRL                  : return KeyScancode::CONTROL_RIGHT;
        case SDL_SCANCODE_RETURN                 : return KeyScancode::RETURN;
        case SDL_SCANCODE_RETURN2                : return KeyScancode::RETURN;
        case SDL_SCANCODE_RGUI                   : return KeyScancode::GUI_RIGHT;
        case SDL_SCANCODE_RIGHT                  : return KeyScancode::ARROW_RIGHT;
        case SDL_SCANCODE_RIGHTBRACKET           : return KeyScancode::BRACKET_RIGHT;
        case SDL_SCANCODE_RSHIFT                 : return KeyScancode::SHIFT_RIGHT;
        case SDL_SCANCODE_SCROLLLOCK             : return KeyScancode::SCROLL_LOCK;
        case SDL_SCANCODE_SELECT                 : return KeyScancode::UNKNOWN;
        case SDL_SCANCODE_SEMICOLON              : return KeyScancode::SEMICOLON;
        case SDL_SCANCODE_SEPARATOR              : return KeyScancode::UNKNOWN;
        case SDL_SCANCODE_SLASH                  : return KeyScancode::SLASH;
        case SDL_SCANCODE_SLEEP                  : return KeyScancode::SLEEP;
        case SDL_SCANCODE_SPACE                  : return KeyScancode::SPACEBAR;
        case SDL_SCANCODE_STOP                   : return KeyScancode::STOP;
        case SDL_SCANCODE_SYSREQ                 : return KeyScancode::SYS_REQ;
        case SDL_SCANCODE_TAB                    : return KeyScancode::TAB;
        case SDL_SCANCODE_THOUSANDSSEPARATOR     : return KeyScancode::UNKNOWN;
        case SDL_SCANCODE_UNDO                   : return KeyScancode::UNKNOWN;
        case SDL_SCANCODE_UNKNOWN                : return KeyScancode::UNKNOWN;
        case SDL_SCANCODE_UP                     : return KeyScancode::ARROW_UP;
        case SDL_SCANCODE_VOLUMEDOWN             : return KeyScancode::SND_VOLUME_DOWN;
        case SDL_SCANCODE_VOLUMEUP               : return KeyScancode::SND_VOLUME_UP;
        case SDL_SCANCODE_WWW                    : return KeyScancode::UNKNOWN;
        default : return KeyScancode::UNKNOWN;
    }
};

}

#endif // PGG_NALINPUTS_HPP
