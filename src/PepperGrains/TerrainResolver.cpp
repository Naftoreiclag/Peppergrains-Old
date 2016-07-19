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

#include "TerrainResolver.hpp"

namespace pgg {
    
TerrainResolver::Patch::Patch(int32_t indX, int32_t indZ)
: mIndX(indX)
, mIndZ(indZ) {
}

TerrainResolver::Patch::~Patch() {
    
}

TerrainResolver::Layer::Layer(uint32_t size)
: mSize(size)
, mFocused(false) {
}

bool TerrainResolver::intersect(int32_t x1, int32_t z1, int32_t x2, int32_t z2, int32_t x3, int32_t z3, int32_t x4, int32_t z4) {
    return !(x3 >= x2 || x4 <= x1 || z3 >= z2 || z4 <= z1);
}

void TerrainResolver::Layer::render(bool useTransition, bool cutSegments, int32_t cutX1, int32_t cutZ1, int32_t cutX2, int32_t cutZ2) {
    for(std::vector<Patch*>::iterator it = mPatches.begin(); it != mPatches.end(); ++ it) {
        Patch* patch = *it;
        int32_t absX = mX + (patch->mIndX * mSize);
        int32_t absZ = mZ + (patch->mIndZ * mSize);
        
        // If patch is "cut" use different rendering technique
        if(cutSegments && intersect(absX, absZ, absX + mSize, absZ + mSize, cutX1, cutZ1, cutX2, cutZ2)) {
            // Devide the patch into 16 sub patches
            for(uint32_t subZ = 0; subZ < 4; ++ subZ) {
                for(uint32_t subX = 0; subX < 4; ++ subX) {
                    
                }
            }
            
            continue;
        }
        
        // If using transitions, and on an edte, use different rendering technique
        if(useTransition) {
            // if(patch->mIndX == 0)...
        }
        
        // Render patch as normal
    }
}

void TerrainResolver::Layer::focus(int32_t focusX, int32_t focusZ) {
    // The absolute coordinates of the patch the focus is located in
    int32_t focusPatchAbsX = focusX - (((focusX % mSize) + mSize) % mSize);
    int32_t focusPatchAbsZ = focusZ - (((focusZ % mSize) + mSize) % mSize);
    
    // The new absolute position for this layer
    int32_t newAbsX = focusPatchAbsX - mSize;
    int32_t newAbsZ = focusPatchAbsZ - mSize;
    
    if(newAbsX == mX && newAbsZ == mZ && mFocused) {
        return;
    }
    
    // Keep track of which patches need to be replaced
    bool needsCreation[9];
    for(int32_t i = 0; i < 9; ++ i) {
        needsCreation[i] = true;
    }
    
    // Update index positions for all patches
    for(std::vector<Patch*>::iterator it = mPatches.begin(); it != mPatches.end(); ++ it) {
        Patch* patch = *it;
        
        patch->mIndX -= (newAbsX - mX) / mSize;
        patch->mIndZ -= (newAbsZ - mZ) / mSize;
        
        // If this patch is outside of the new bounds, remove it
        if(patch->mIndX < 0 || patch->mIndX >= 3 || patch->mIndZ < 0 || patch->mIndZ >= 3) {
            it = mPatches.erase(it);
            delete patch;
            continue;
        }
        
        // Just keep it, instead of deleting it
        needsCreation[patch->mIndZ * 3 + patch->mIndX] = false;
    }
    
    // Replace all missing patches
    for(int32_t i = 0; i < 9; ++ i) {
        if(needsCreation[i]) {
            int32_t indX = i % 3;
            int32_t indZ = (i - indX) / 3;
            
            mPatches.push_back(new Patch(indX, indZ));
        }
    }
    
    mFocused = true;
}

TerrainResolver::Layer::~Layer() {
    
}
    
TerrainResolver::TerrainResolver(int32_t x, int32_t y) {
    mLayers.push_back(new Layer(1));
    mLayers.push_back(new Layer(4));
    mLayers.push_back(new Layer(16));
    mLayers.push_back(new Layer(64));
    
    mFocusX = x;
    mFocusY = y;
    
    onFocusUpdate();
}

TerrainResolver::~TerrainResolver() {
    for(std::vector<Layer*>::iterator it = mLayers.begin(); it != mLayers.end(); ++ it) {
        delete *it;
    }
}

void TerrainResolver::onFocusUpdate() {
    for(std::vector<Layer*>::iterator it = mLayers.begin(); it != mLayers.end(); ++ it) {
        Layer* layer = *it;
        layer->focus(mFocusX, mFocusY);
    }
}

void TerrainResolver::setFocus(int32_t x, int32_t y) {
    if(mFocusX == x && mFocusY == y) {
        return;
    }
    
    mFocusX = x;
    mFocusY = y;
    
    onFocusUpdate();
}

void TerrainResolver::render() {
    for(std::vector<Layer*>::iterator it = mLayers.begin(); it != mLayers.end(); ++ it) {
        Layer* current = *it;
        
    }
}

}

