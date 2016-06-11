/*
   Copyright 2015-2016 James Fong

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

#include "GameLayer.hpp"

namespace pgg {

GameLayer::GameLayer() {}
GameLayer::~GameLayer() {}

// Lifecycle
void GameLayer::onBegin() {}
void GameLayer::onEnd() {}

// Ticks
void GameLayer::onTick(float tpf, const InputState* keyStates) {}

/* Key filtering:
 *  Set whatever keystates to be false before passing them on to the next layers
 *  Return true to set all keys to false
 *  Not guaranteed to be called each tick
 */
bool GameLayer::filterKeys(InputState* keyStates) { return false; }

// Layering
void GameLayer::onAddedAbove(const GameLayer* layer) {}
void GameLayer::onRemovedAbove(const GameLayer* layer) {}

// Key handling
bool GameLayer::onKeyboardEvent(const KeyboardEvent& event) { return false; }
bool GameLayer::onTextInput(const TextInputEvent& event) { return false; }

// Mouse handling
bool GameLayer::onMouseMove(const MouseMoveEvent& event) { return false; }
bool GameLayer::onMouseButton(const MouseButtonEvent& event) { return false; }
bool GameLayer::onMouseWheel(const MouseWheelMoveEvent& event) { return false; }

// Windowing
bool GameLayer::onWindowSizeUpdate(const WindowResizeEvent& event) { return false; }

bool GameLayer::onQuit(const QuitEvent& event) { return false; }

}

