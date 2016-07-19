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

#ifndef PGG_TERRAINRESOLVER_HPP
#define PGG_TERRAINRESOLVER_HPP

#include <stdint.h>
#include <vector>

namespace pgg {

// Handles terrain resolution/lod
class TerrainResolver {
private:
    // Note points 2 and 4 are non-inclusive
    static bool intersect(int32_t x1, int32_t z1, int32_t x2, int32_t z2, int32_t x3, int32_t z3, int32_t x4, int32_t z4);
    
    class Patch {
    public:
        int32_t mIndX;
        int32_t mIndZ;
    
        Patch(int32_t indX, int32_t indZ);
        ~Patch();
    };
    
    class Layer {
    private:
        int32_t mX;
        int32_t mZ;
        
        bool mFocused;
    public:
        void focus(int32_t x, int32_t z);
        
        // Size of patches
        const int32_t mSize;
    
        std::vector<Patch*> mPatches;
        
        Layer(uint32_t size);
        ~Layer();
        
        void render(bool useTransition, bool cutSegments, int32_t cutX1, int32_t cutZ1, int32_t cutX2, int32_t cutZ2);
    };
    
    std::vector<Layer*> mLayers;
    
    int32_t mFocusX;
    int32_t mFocusY;
    
    void onFocusUpdate();
    
    
public:
    TerrainResolver(int32_t x, int32_t z);
    ~TerrainResolver();
    
    void setFocus(int32_t x, int32_t z);
    void render();
};

}

#endif // PGG_TERRAINRESOLVER_HPP
