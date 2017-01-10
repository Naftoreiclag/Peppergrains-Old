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

#include "Gamelayer.hpp"

namespace pgg {

Gamelayer::Gamelayer() {}
Gamelayer::~Gamelayer() {}

// Lifecycle
void Gamelayer::onBegin() {}
void Gamelayer::onEnd() {}

// Ticks
void Gamelayer::onTick(double tpf, const InputState* keyStates) {}

/* Key filtering:
 *  Set whatever keystates to be false before passing them on to the next layers
 *  Return true to set all keys to false
 *  Not guaranteed to be called each tick
 */
bool Gamelayer::filterKeys(InputState* keyStates) { return false; }

// Layering
void Gamelayer::onAddedAbove(const Gamelayer* layer) {}
void Gamelayer::onRemovedAbove(const Gamelayer* layer) {}

// Key handling
bool Gamelayer::onKeyboardEvent(const KeyboardEvent& event) { return false; }
bool Gamelayer::onTextInput(const TextInputEvent& event) { return false; }

// Mouse handling
bool Gamelayer::onMouseMove(const MouseMoveEvent& event) { return false; }
bool Gamelayer::onMouseButton(const MouseButtonEvent& event) { return false; }
bool Gamelayer::onMouseWheel(const MouseWheelMoveEvent& event) { return false; }

// Windowing
bool Gamelayer::onWindowSizeUpdate(const WindowResizeEvent& event) { return false; }

bool Gamelayer::onQuit(const QuitEvent& event) { return false; }

}

