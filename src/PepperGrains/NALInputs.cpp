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

#include "NALInputs.hpp"

namespace pgg {
    
namespace Input {


KeyScancode mouseButtonFromSDL(uint8_t button) {
    if(button == SDL_BUTTON_LEFT) { return  KeyScancode::M_LEFT; }
    else if(button == SDL_BUTTON_RIGHT) { return KeyScancode::M_RIGHT; }
    else if(button == SDL_BUTTON_MIDDLE) { return KeyScancode::M_MIDDLE; }
    else if(button == SDL_BUTTON_X1 ) { return KeyScancode::M_SPECIAL_0; }
    else if(button == SDL_BUTTON_X2 ) { return KeyScancode::M_SPECIAL_1; }
    return KeyScancode::UNKNOWN;
}
    
KeyScancode keyScancodeFromSDL(SDL_Scancode key) {
    switch(key) {
        case SDL_SCANCODE_0                      : return KeyScancode::K_NUM_0;
        case SDL_SCANCODE_1                      : return KeyScancode::K_NUM_1;
        case SDL_SCANCODE_2                      : return KeyScancode::K_NUM_2;
        case SDL_SCANCODE_3                      : return KeyScancode::K_NUM_3;
        case SDL_SCANCODE_4                      : return KeyScancode::K_NUM_4;
        case SDL_SCANCODE_5                      : return KeyScancode::K_NUM_5;
        case SDL_SCANCODE_6                      : return KeyScancode::K_NUM_6;
        case SDL_SCANCODE_7                      : return KeyScancode::K_NUM_7;
        case SDL_SCANCODE_8                      : return KeyScancode::K_NUM_8;
        case SDL_SCANCODE_9                      : return KeyScancode::K_NUM_9;
        case SDL_SCANCODE_A                      : return KeyScancode::K_A;
        case SDL_SCANCODE_B                      : return KeyScancode::K_B;
        case SDL_SCANCODE_C                      : return KeyScancode::K_C;
        case SDL_SCANCODE_D                      : return KeyScancode::K_D;
        case SDL_SCANCODE_E                      : return KeyScancode::K_E;
        case SDL_SCANCODE_F                      : return KeyScancode::K_F;
        case SDL_SCANCODE_G                      : return KeyScancode::K_G;
        case SDL_SCANCODE_H                      : return KeyScancode::K_H;
        case SDL_SCANCODE_I                      : return KeyScancode::K_I;
        case SDL_SCANCODE_J                      : return KeyScancode::K_J;
        case SDL_SCANCODE_K                      : return KeyScancode::K_K;
        case SDL_SCANCODE_L                      : return KeyScancode::K_L;
        case SDL_SCANCODE_M                      : return KeyScancode::K_M;
        case SDL_SCANCODE_N                      : return KeyScancode::K_N;
        case SDL_SCANCODE_O                      : return KeyScancode::K_O;
        case SDL_SCANCODE_P                      : return KeyScancode::K_P;
        case SDL_SCANCODE_Q                      : return KeyScancode::K_Q;
        case SDL_SCANCODE_R                      : return KeyScancode::K_R;
        case SDL_SCANCODE_S                      : return KeyScancode::K_S;
        case SDL_SCANCODE_T                      : return KeyScancode::K_T;
        case SDL_SCANCODE_U                      : return KeyScancode::K_U;
        case SDL_SCANCODE_V                      : return KeyScancode::K_V;
        case SDL_SCANCODE_W                      : return KeyScancode::K_W;
        case SDL_SCANCODE_X                      : return KeyScancode::K_X;
        case SDL_SCANCODE_Y                      : return KeyScancode::K_Y;
        case SDL_SCANCODE_Z                      : return KeyScancode::K_Z;
        case SDL_SCANCODE_KP_0                   : return KeyScancode::K_NUMPAD_0;
        case SDL_SCANCODE_KP_1                   : return KeyScancode::K_NUMPAD_1;
        case SDL_SCANCODE_KP_2                   : return KeyScancode::K_NUMPAD_2;
        case SDL_SCANCODE_KP_3                   : return KeyScancode::K_NUMPAD_3;
        case SDL_SCANCODE_KP_4                   : return KeyScancode::K_NUMPAD_4;
        case SDL_SCANCODE_KP_5                   : return KeyScancode::K_NUMPAD_5;
        case SDL_SCANCODE_KP_6                   : return KeyScancode::K_NUMPAD_6;
        case SDL_SCANCODE_KP_7                   : return KeyScancode::K_NUMPAD_7;
        case SDL_SCANCODE_KP_8                   : return KeyScancode::K_NUMPAD_8;
        case SDL_SCANCODE_KP_9                   : return KeyScancode::K_NUMPAD_9;
        case SDL_SCANCODE_KP_00                  : return KeyScancode::K_NUMPAD_00;
        case SDL_SCANCODE_KP_000                 : return KeyScancode::K_NUMPAD_000;
        case SDL_SCANCODE_KP_A                   : return KeyScancode::K_NUMPAD_A;
        case SDL_SCANCODE_KP_B                   : return KeyScancode::K_NUMPAD_B;
        case SDL_SCANCODE_KP_C                   : return KeyScancode::K_NUMPAD_C;
        case SDL_SCANCODE_KP_D                   : return KeyScancode::K_NUMPAD_D;
        case SDL_SCANCODE_KP_E                   : return KeyScancode::K_NUMPAD_E;
        case SDL_SCANCODE_KP_F                   : return KeyScancode::K_NUMPAD_F;
        case SDL_SCANCODE_KP_AMPERSAND           : return KeyScancode::K_NUMPAD_AMPERSAND;
        case SDL_SCANCODE_KP_AT                  : return KeyScancode::K_NUMPAD_AT;
        case SDL_SCANCODE_KP_BACKSPACE           : return KeyScancode::K_NUMPAD_BACKSPACE;
        case SDL_SCANCODE_KP_BINARY              : return KeyScancode::K_NUMPAD_BINARY;
        case SDL_SCANCODE_KP_CLEAR               : return KeyScancode::K_NUMPAD_CLEAR;
        case SDL_SCANCODE_KP_CLEARENTRY          : return KeyScancode::K_NUMPAD_CLEAR_ENTRY;
        case SDL_SCANCODE_KP_COLON               : return KeyScancode::K_NUMPAD_COLON;
        case SDL_SCANCODE_KP_COMMA               : return KeyScancode::K_NUMPAD_COMMA;
        case SDL_SCANCODE_KP_DBLAMPERSAND        : return KeyScancode::K_NUMPAD_DOUBLE_AMPERSAND;
        case SDL_SCANCODE_KP_DBLVERTICALBAR      : return KeyScancode::K_NUMPAD_DOUBLE_VERTICAL;
        case SDL_SCANCODE_KP_DECIMAL             : return KeyScancode::K_NUMPAD_DECIMAL_POINT;
        case SDL_SCANCODE_KP_DIVIDE              : return KeyScancode::K_NUMPAD_DIVIDE;
        case SDL_SCANCODE_KP_ENTER               : return KeyScancode::K_NUMPAD_ENTER;
        case SDL_SCANCODE_KP_EQUALS              : return KeyScancode::K_NUMPAD_EQUALS;
        case SDL_SCANCODE_KP_EQUALSAS400         : return KeyScancode::K_NUMPAD_EQUALS_ALT;
        case SDL_SCANCODE_KP_EXCLAM              : return KeyScancode::K_NUMPAD_FACTORIAL;
        case SDL_SCANCODE_KP_GREATER             : return KeyScancode::K_NUMPAD_GREATER_THAN;
        case SDL_SCANCODE_KP_HASH                : return KeyScancode::K_NUMPAD_HASH;
        case SDL_SCANCODE_KP_HEXADECIMAL         : return KeyScancode::K_NUMPAD_HEX;
        case SDL_SCANCODE_KP_LEFTBRACE           : return KeyScancode::K_NUMPAD_BRACE_LEFT;
        case SDL_SCANCODE_KP_LEFTPAREN           : return KeyScancode::K_NUMPAD_PARENTHESIS_LEFT;
        case SDL_SCANCODE_KP_LESS                : return KeyScancode::K_NUMPAD_LESS_THAN;
        case SDL_SCANCODE_KP_MEMADD              : return KeyScancode::K_NUMPAD_MEMORY_ADD;
        case SDL_SCANCODE_KP_MEMCLEAR            : return KeyScancode::K_NUMPAD_MEMORY_CLEAR;
        case SDL_SCANCODE_KP_MEMDIVIDE           : return KeyScancode::K_NUMPAD_MEMORY_DIVIDE;
        case SDL_SCANCODE_KP_MEMMULTIPLY         : return KeyScancode::K_NUMPAD_MEMORY_MULTIPLY;
        case SDL_SCANCODE_KP_MEMRECALL           : return KeyScancode::K_NUMPAD_MEMORY_CALL;
        case SDL_SCANCODE_KP_MEMSTORE            : return KeyScancode::K_NUMPAD_MEMORY_STORE;
        case SDL_SCANCODE_KP_MEMSUBTRACT         : return KeyScancode::K_NUMPAD_MEMORY_SUBTRACT;
        case SDL_SCANCODE_KP_MINUS               : return KeyScancode::K_NUMPAD_SUBTRACT;
        case SDL_SCANCODE_KP_MULTIPLY            : return KeyScancode::K_NUMPAD_MULTIPLY;
        case SDL_SCANCODE_KP_OCTAL               : return KeyScancode::K_NUMPAD_OCT;
        case SDL_SCANCODE_KP_PERCENT             : return KeyScancode::K_NUMPAD_PERCENT;
        case SDL_SCANCODE_KP_PERIOD              : return KeyScancode::K_NUMPAD_PERIOD;
        case SDL_SCANCODE_KP_PLUS                : return KeyScancode::K_NUMPAD_ADD;
        case SDL_SCANCODE_KP_PLUSMINUS           : return KeyScancode::K_NUMPAD_PLUS_MINUS;
        case SDL_SCANCODE_KP_POWER               : return KeyScancode::K_NUMPAD_CARET;
        case SDL_SCANCODE_KP_RIGHTBRACE          : return KeyScancode::K_NUMPAD_BRACE_RIGHT;
        case SDL_SCANCODE_KP_RIGHTPAREN          : return KeyScancode::K_NUMPAD_PARENTHESIS_RIGHT;
        case SDL_SCANCODE_KP_SPACE               : return KeyScancode::K_NUMPAD_SPACE;
        case SDL_SCANCODE_KP_TAB                 : return KeyScancode::K_NUMPAD_TAB;
        case SDL_SCANCODE_KP_VERTICALBAR         : return KeyScancode::K_NUMPAD_VERTICAL;
        case SDL_SCANCODE_KP_XOR                 : return KeyScancode::K_NUMPAD_XOR;
        case SDL_SCANCODE_F1                     : return KeyScancode::K_F1;
        case SDL_SCANCODE_F2                     : return KeyScancode::K_F2;
        case SDL_SCANCODE_F3                     : return KeyScancode::K_F3;
        case SDL_SCANCODE_F4                     : return KeyScancode::K_F4;
        case SDL_SCANCODE_F5                     : return KeyScancode::K_F5;
        case SDL_SCANCODE_F6                     : return KeyScancode::K_F6;
        case SDL_SCANCODE_F7                     : return KeyScancode::K_F7;
        case SDL_SCANCODE_F8                     : return KeyScancode::K_F8;
        case SDL_SCANCODE_F9                     : return KeyScancode::K_F9;
        case SDL_SCANCODE_F10                    : return KeyScancode::K_F10;
        case SDL_SCANCODE_F11                    : return KeyScancode::K_F11;
        case SDL_SCANCODE_F12                    : return KeyScancode::K_F12;
        case SDL_SCANCODE_F13                    : return KeyScancode::K_F13;
        case SDL_SCANCODE_F14                    : return KeyScancode::K_F14;
        case SDL_SCANCODE_F15                    : return KeyScancode::K_F15;
        case SDL_SCANCODE_F16                    : return KeyScancode::K_F16;
        case SDL_SCANCODE_F17                    : return KeyScancode::K_F17;
        case SDL_SCANCODE_F18                    : return KeyScancode::K_F18;
        case SDL_SCANCODE_F19                    : return KeyScancode::K_F19;
        case SDL_SCANCODE_F20                    : return KeyScancode::K_F20;
        case SDL_SCANCODE_F21                    : return KeyScancode::K_F21;
        case SDL_SCANCODE_F22                    : return KeyScancode::K_F22;
        case SDL_SCANCODE_F23                    : return KeyScancode::K_F23;
        case SDL_SCANCODE_F24                    : return KeyScancode::K_F24;
        case SDL_SCANCODE_AC_BACK                : return KeyScancode::K_APP_BACK;
        case SDL_SCANCODE_AC_BOOKMARKS           : return KeyScancode::K_APP_FAVORITES;
        case SDL_SCANCODE_AC_FORWARD             : return KeyScancode::K_APP_FORWARD;
        case SDL_SCANCODE_AC_HOME                : return KeyScancode::K_APP_HOME;
        case SDL_SCANCODE_AC_REFRESH             : return KeyScancode::K_APP_REFRESH;
        case SDL_SCANCODE_AC_SEARCH              : return KeyScancode::K_APP_SEARCH;
        case SDL_SCANCODE_AC_STOP                : return KeyScancode::K_APP_STOP;
        case SDL_SCANCODE_AGAIN                  : return KeyScancode::K_REDO;
        case SDL_SCANCODE_ALTERASE               : return KeyScancode::K_ERASE;
        case SDL_SCANCODE_APOSTROPHE             : return KeyScancode::K_APOSTROPHE;
        case SDL_SCANCODE_APPLICATION            : return KeyScancode::K_APP_MENU;
        case SDL_SCANCODE_AUDIOMUTE              : return KeyScancode::K_SND_MUTE;
        case SDL_SCANCODE_AUDIONEXT              : return KeyScancode::K_SND_NEXT;
        case SDL_SCANCODE_AUDIOPLAY              : return KeyScancode::K_SND_PLAY;
        case SDL_SCANCODE_AUDIOPREV              : return KeyScancode::K_SND_PREV;
        case SDL_SCANCODE_AUDIOSTOP              : return KeyScancode::K_SND_STOP;
        case SDL_SCANCODE_BACKSLASH              : return KeyScancode::K_BACKSLASH;
        case SDL_SCANCODE_BACKSPACE              : return KeyScancode::K_BACKSPACE;
        case SDL_SCANCODE_BRIGHTNESSDOWN         : return KeyScancode::K_BRIGHTNESS_DOWN;
        case SDL_SCANCODE_BRIGHTNESSUP           : return KeyScancode::K_BRIGHTNESS_UP;
        case SDL_SCANCODE_CALCULATOR             : return KeyScancode::K_CALCULATOR;
        case SDL_SCANCODE_CANCEL                 : return KeyScancode::K_CANCEL;
        case SDL_SCANCODE_CAPSLOCK               : return KeyScancode::K_CAPS_LOCK;
        case SDL_SCANCODE_CLEAR                  : return KeyScancode::K_CLEAR;
        case SDL_SCANCODE_CLEARAGAIN             : return KeyScancode::K_CLEAR_AGAIN;
        case SDL_SCANCODE_COMMA                  : return KeyScancode::K_COMMA;
        case SDL_SCANCODE_COMPUTER               : return KeyScancode::K_MY_COMPUTER;
        case SDL_SCANCODE_COPY                   : return KeyScancode::K_COPY;
        case SDL_SCANCODE_CRSEL                  : return KeyScancode::K_CR_CEL;
        case SDL_SCANCODE_CURRENCYSUBUNIT        : return KeyScancode::K_CURRENCY_SUB_UNIT;
        case SDL_SCANCODE_CURRENCYUNIT           : return KeyScancode::K_CURRENCY_UNIT;
        case SDL_SCANCODE_CUT                    : return KeyScancode::K_CUT;
        case SDL_SCANCODE_DECIMALSEPARATOR       : return KeyScancode::K_DECIMAL_POINT;
        case SDL_SCANCODE_DELETE                 : return KeyScancode::K_DELETE;
        case SDL_SCANCODE_DISPLAYSWITCH          : return KeyScancode::K_VIDEO_MODE_SWITCH;
        case SDL_SCANCODE_DOWN                   : return KeyScancode::K_ARROW_DOWN;
        case SDL_SCANCODE_EJECT                  : return KeyScancode::K_EJECT;
        case SDL_SCANCODE_END                    : return KeyScancode::K_END;
        case SDL_SCANCODE_EQUALS                 : return KeyScancode::K_EQUALS;
        case SDL_SCANCODE_ESCAPE                 : return KeyScancode::K_ESCAPE;
        case SDL_SCANCODE_EXECUTE                : return KeyScancode::K_EXECUTE;
        case SDL_SCANCODE_EXSEL                  : return KeyScancode::K_EX_SEL;
        case SDL_SCANCODE_FIND                   : return KeyScancode::K_FIND;
        case SDL_SCANCODE_GRAVE                  : return KeyScancode::K_GRAVE;
        case SDL_SCANCODE_HELP                   : return KeyScancode::K_HELP;
        case SDL_SCANCODE_HOME                   : return KeyScancode::K_HOME;
        case SDL_SCANCODE_INSERT                 : return KeyScancode::K_INSERT;
        case SDL_SCANCODE_INTERNATIONAL1         : return KeyScancode::K_INTERNATIONAL_1;
        case SDL_SCANCODE_INTERNATIONAL2         : return KeyScancode::K_INTERNATIONAL_2;
        case SDL_SCANCODE_INTERNATIONAL3         : return KeyScancode::K_INTERNATIONAL_3;
        case SDL_SCANCODE_INTERNATIONAL4         : return KeyScancode::K_INTERNATIONAL_4;
        case SDL_SCANCODE_INTERNATIONAL5         : return KeyScancode::K_INTERNATIONAL_5;
        case SDL_SCANCODE_INTERNATIONAL6         : return KeyScancode::K_INTERNATIONAL_6;
        case SDL_SCANCODE_INTERNATIONAL7         : return KeyScancode::K_INTERNATIONAL_7;
        case SDL_SCANCODE_INTERNATIONAL8         : return KeyScancode::K_INTERNATIONAL_8;
        case SDL_SCANCODE_INTERNATIONAL9         : return KeyScancode::K_INTERNATIONAL_9;
        case SDL_SCANCODE_KBDILLUMDOWN           : return KeyScancode::K_ILLUMINATION_DOWN;
        case SDL_SCANCODE_KBDILLUMTOGGLE         : return KeyScancode::K_ILLUMINATION_TOGGLE;
        case SDL_SCANCODE_KBDILLUMUP             : return KeyScancode::K_ILLUMINATION_UP;
        case SDL_SCANCODE_LALT                   : return KeyScancode::K_ALT_LEFT;
        case SDL_SCANCODE_LANG1                  : return KeyScancode::K_LANG_1;
        case SDL_SCANCODE_LANG2                  : return KeyScancode::K_LANG_2;
        case SDL_SCANCODE_LANG3                  : return KeyScancode::K_LANG_3;
        case SDL_SCANCODE_LANG4                  : return KeyScancode::K_LANG_4;
        case SDL_SCANCODE_LANG5                  : return KeyScancode::K_LANG_5;
        case SDL_SCANCODE_LANG6                  : return KeyScancode::K_LANG_6;
        case SDL_SCANCODE_LANG7                  : return KeyScancode::K_LANG_7;
        case SDL_SCANCODE_LANG8                  : return KeyScancode::K_LANG_8;
        case SDL_SCANCODE_LANG9                  : return KeyScancode::K_LANG_9;
        case SDL_SCANCODE_LCTRL                  : return KeyScancode::K_CONTROL_LEFT;
        case SDL_SCANCODE_LEFT                   : return KeyScancode::K_ARROW_LEFT;
        case SDL_SCANCODE_LEFTBRACKET            : return KeyScancode::K_BRACKET_LEFT;
        case SDL_SCANCODE_LGUI                   : return KeyScancode::K_GUI_LEFT;
        case SDL_SCANCODE_LSHIFT                 : return KeyScancode::K_SHIFT_LEFT;
        case SDL_SCANCODE_MAIL                   : return KeyScancode::K_MAIL;
        case SDL_SCANCODE_MEDIASELECT            : return KeyScancode::K_MEDIA_SELECT;
        case SDL_SCANCODE_MENU                   : return KeyScancode::K_MENU;
        case SDL_SCANCODE_MINUS                  : return KeyScancode::K_SUBTRACT;
        case SDL_SCANCODE_MODE                   : return KeyScancode::K_MODE_SWITCH;
        case SDL_SCANCODE_MUTE                   : return KeyScancode::K_MUTE;
        case SDL_SCANCODE_NONUSBACKSLASH         : return KeyScancode::UNKNOWN;
        case SDL_SCANCODE_NONUSHASH              : return KeyScancode::UNKNOWN;
        case SDL_SCANCODE_NUMLOCKCLEAR           : return KeyScancode::K_NUMLOCK;
        case SDL_SCANCODE_OPER                   : return KeyScancode::K_OPER;
        case SDL_SCANCODE_OUT                    : return KeyScancode::K_OUT;
        case SDL_SCANCODE_PAGEDOWN               : return KeyScancode::K_PAGE_DOWN;
        case SDL_SCANCODE_PAGEUP                 : return KeyScancode::K_PAGE_UP;
        case SDL_SCANCODE_PASTE                  : return KeyScancode::K_PASTE;
        case SDL_SCANCODE_PAUSE                  : return KeyScancode::K_PAUSE;
        case SDL_SCANCODE_PERIOD                 : return KeyScancode::K_PERIOD;
        case SDL_SCANCODE_POWER                  : return KeyScancode::K_POWER;
        case SDL_SCANCODE_PRINTSCREEN            : return KeyScancode::K_PRINTSCREEN;
        case SDL_SCANCODE_PRIOR                  : return KeyScancode::K_PRIOR;
        case SDL_SCANCODE_RALT                   : return KeyScancode::K_ALT_RIGHT;
        case SDL_SCANCODE_RCTRL                  : return KeyScancode::K_CONTROL_RIGHT;
        case SDL_SCANCODE_RETURN                 : return KeyScancode::K_RETURN;
        case SDL_SCANCODE_RETURN2                : return KeyScancode::K_RETURN_ALT;
        case SDL_SCANCODE_RGUI                   : return KeyScancode::K_GUI_RIGHT;
        case SDL_SCANCODE_RIGHT                  : return KeyScancode::K_ARROW_RIGHT;
        case SDL_SCANCODE_RIGHTBRACKET           : return KeyScancode::K_BRACKET_RIGHT;
        case SDL_SCANCODE_RSHIFT                 : return KeyScancode::K_SHIFT_RIGHT;
        case SDL_SCANCODE_SCROLLLOCK             : return KeyScancode::K_SCROLL_LOCK;
        case SDL_SCANCODE_SELECT                 : return KeyScancode::K_SELECT;
        case SDL_SCANCODE_SEMICOLON              : return KeyScancode::K_SEMICOLON;
        case SDL_SCANCODE_SEPARATOR              : return KeyScancode::K_SEPARATOR;
        case SDL_SCANCODE_SLASH                  : return KeyScancode::K_SLASH;
        case SDL_SCANCODE_SLEEP                  : return KeyScancode::K_SLEEP;
        case SDL_SCANCODE_SPACE                  : return KeyScancode::K_SPACEBAR;
        case SDL_SCANCODE_STOP                   : return KeyScancode::K_STOP;
        case SDL_SCANCODE_SYSREQ                 : return KeyScancode::K_SYS_REQ;
        case SDL_SCANCODE_TAB                    : return KeyScancode::K_TAB;
        case SDL_SCANCODE_THOUSANDSSEPARATOR     : return KeyScancode::K_THOUSANDS_SEPARATOR;
        case SDL_SCANCODE_UNDO                   : return KeyScancode::K_UNDO;
        case SDL_SCANCODE_UP                     : return KeyScancode::K_ARROW_UP;
        case SDL_SCANCODE_VOLUMEDOWN             : return KeyScancode::K_SND_VOLUME_DOWN;
        case SDL_SCANCODE_VOLUMEUP               : return KeyScancode::K_SND_VOLUME_UP;
        case SDL_SCANCODE_WWW                    : return KeyScancode::K_WORLD_WIDE_WEB;
        default : return KeyScancode::UNKNOWN;
    }
}


}

}
