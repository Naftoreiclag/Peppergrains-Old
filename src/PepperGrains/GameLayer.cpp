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
void GameLayer::onTick(float tps, const Uint8* keyStates) {}

/* Key filtering:
 *  Set whatever keystates to be false before passing them on to the next layers
 *  Return true to set all keys to false
 *  Not guaranteed to be called each tick
 */
bool GameLayer::filterKeys(Uint8* keyStates) { return false; }

// Layering
void GameLayer::onAddedAbove(const GameLayer* layer) {}
void GameLayer::onRemovedAbove(const GameLayer* layer) {}

// Key handling
bool GameLayer::onKeyPress(const SDL_KeyboardEvent& event, bool repeat) { return false; }
bool GameLayer::onKeyRelease(const SDL_KeyboardEvent& event) { return false; }
bool GameLayer::onTextInput(const SDL_TextInputEvent& event) { return false; }

// Mouse handling
bool GameLayer::onMouseMove(const SDL_MouseMotionEvent& event) { return false; }
bool GameLayer::onMousePress(const SDL_MouseButtonEvent& event) { return false; }
bool GameLayer::onMouseRelease(const SDL_MouseButtonEvent& event) { return false; }
bool GameLayer::onMouseWheel(const SDL_MouseWheelEvent& event) { return false; }

// Windowing
bool GameLayer::onWindowSizeUpdate(const SDL_WindowEvent& event) { return false; }

}

