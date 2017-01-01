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

#include "GamelayerMachine.hpp"

#include <algorithm>
#include <cassert>

#include "Gamelayer.hpp"

namespace pgg {
    
InputState GamelayerMachine::sEmptyInputState;

GamelayerMachine::GamelayerMachine() { }

GamelayerMachine::~GamelayerMachine() {}

void GamelayerMachine::addBottom(Gamelayer* addMe) {
    mLayers.insert(mLayers.begin(), addMe);
    addMe->onBegin();
}

void GamelayerMachine::addAbove(Gamelayer* addMe, Gamelayer* caller) {
    // Find where the caller is located
    std::vector<Gamelayer*>::iterator location = mLayers.end();
    for(std::vector<Gamelayer*>::iterator iter = mLayers.begin(); iter != mLayers.end(); ++ iter) {
        Gamelayer* layer = *iter;
        
        if(layer == caller) {
            location = iter;
            break;
        }
    }
    
    // The caller is valid
    assert(location != mLayers.end() && "Adding Gamelayer in unknown position");
    
    // Insert the new game layer into the next location ("One layer above")
    ++ location;
    mLayers.insert(location, addMe); // Inserting here is safe
    addMe->onBegin();
    
    // Inform all layers "below" this one that a new layer was added above them (this should logically include the caller)
    for(std::vector<Gamelayer*>::iterator iter = mLayers.begin(); iter != mLayers.end(); ++ iter) {
        Gamelayer* layer = *iter;
        
        if(layer == addMe) {
            break;
        }
        else {
            layer->onAddedAbove(addMe);
        }
    }
}
void GamelayerMachine::remove(Gamelayer* removeMe) {
    // Find the layer to remove
    std::vector<Gamelayer*>::iterator location = mLayers.end();
    for(std::vector<Gamelayer*>::iterator iter = mLayers.begin(); iter != mLayers.end(); ++ iter) {
        Gamelayer* layer = *iter;
        
        if(layer == removeMe) {
            location = iter;
            break;
        }
    }
    
    // This layer was found
    assert(location != mLayers.end() && "Removing Gamelayer that has not yet been added");
    
    // Inform layer that it is going to be removed
    removeMe->onEnd();
    
    // Inform all layers "below" this one that a layer above them was removed
    for(std::vector<Gamelayer*>::iterator iter = mLayers.begin(); iter != location; ++ iter) {
        Gamelayer* layer = *iter;
        
        layer->onRemovedAbove(removeMe);
    }
    
    // Remove layer
    mLayers.erase(location);
}
void GamelayerMachine::removeAll() {
    std::vector<Gamelayer*>::reverse_iterator iter = mLayers.rbegin();
    while(iter != mLayers.rend()) {
        Gamelayer* layer = *iter;
        this->remove(layer);
        ++ iter;
    }
}

// Ticks
void GamelayerMachine::onTick(float tpf, const InputState* keys) {
    bool allKeysFiltered = false;
    InputState filteredInputState = *keys;
    
    std::vector<Gamelayer*>::reverse_iterator iter = mLayers.rbegin();
    while(iter != mLayers.rend()) {
        Gamelayer* layer = *iter;
        
        if(allKeysFiltered) {
            layer->onTick(tpf, &sEmptyInputState);
            
            ++ iter;
        }
        else {
            layer->onTick(tpf, &filteredInputState);
            
            // Only filter keys if there are layers below this one
            if(++ iter != mLayers.rend()) {
                if(layer->filterKeys(&filteredInputState)) {
                    allKeysFiltered = true;
                }
            }
        }
    }
}

// Key handling
void GamelayerMachine::onKeyboardEvent(const KeyboardEvent& event) {
    for(std::vector<Gamelayer*>::reverse_iterator iter = mLayers.rbegin(); iter != mLayers.rend(); ++ iter) {
        Gamelayer* layer = *iter;
        
        // Blocking
        if(layer->onKeyboardEvent(event)) {
            break;
        }
    }
}
void GamelayerMachine::onTextInput(const TextInputEvent& event) {
    for(std::vector<Gamelayer*>::reverse_iterator iter = mLayers.rbegin(); iter != mLayers.rend(); ++ iter) {
        Gamelayer* layer = *iter;
        
        // Blocking
        if(layer->onTextInput(event)) {
            break;
        }
    }
}

// Mouse handling
void GamelayerMachine::onMouseMove(const MouseMoveEvent& event) {
    for(std::vector<Gamelayer*>::reverse_iterator iter = mLayers.rbegin(); iter != mLayers.rend(); ++ iter) {
        Gamelayer* layer = *iter;
        
        // Blocking
        if(layer->onMouseMove(event)) {
            break;
        }
    }
}
void GamelayerMachine::onMouseButton(const MouseButtonEvent& event) {
    for(std::vector<Gamelayer*>::reverse_iterator iter = mLayers.rbegin(); iter != mLayers.rend(); ++ iter) {
        Gamelayer* layer = *iter;
        
        // Blocking
        if(layer->onMouseButton(event)) {
            break;
        }
    }
}
void GamelayerMachine::onMouseWheel(const MouseWheelMoveEvent& event) {
    for(std::vector<Gamelayer*>::reverse_iterator iter = mLayers.rbegin(); iter != mLayers.rend(); ++ iter) {
        Gamelayer* layer = *iter;
        
        // Blocking
        if(layer->onMouseWheel(event)) {
            break;
        }
    }
}
void GamelayerMachine::onWindowSizeUpdate(const WindowResizeEvent& event) {
    for(std::vector<Gamelayer*>::reverse_iterator iter = mLayers.rbegin(); iter != mLayers.rend(); ++ iter) {
        Gamelayer* layer = *iter;
        
        // Blocking
        if(layer->onWindowSizeUpdate(event)) {
            break;
        }
    }
}
void GamelayerMachine::onQuit(const QuitEvent& event) {
    for(std::vector<Gamelayer*>::reverse_iterator iter = mLayers.rbegin(); iter != mLayers.rend(); ++ iter) {
        Gamelayer* layer = *iter;
        
        // Blocking
        if(layer->onQuit(event)) {
            break;
        }
    }
}

}
