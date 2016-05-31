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

#include "SDLRendererData.hpp"

#include <stdint.h>

namespace pgg {
    SDLRendererData* SDLRendererData::getSingleton() {
        static SDLRendererData instance;
        return &instance;
    }
    
    SDLRendererData::SDLRendererData() {
        
    }
    
    SDLRendererData::~SDLRendererData() {
        
    }
    
    void SDLRendererData::loadData(SDL_Renderer* renderer) {
        SDL_RendererInfo rendererInfo;
        SDL_GetRendererInfo(renderer, &rendererInfo);
        
        mInfo.name = std::string(rendererInfo.name);
        
        uint32_t flags = rendererInfo.flags;
        mInfo.softwareFallback = (flags & SDL_RENDERER_SOFTWARE) != 0;
        mInfo.hardwareAccelerated = (flags & SDL_RENDERER_ACCELERATED) != 0;
        mInfo.supportTextureRender = (flags & SDL_RENDERER_TARGETTEXTURE) != 0;
    }
    
    const SDLRendererData::SDLInfo& SDLRendererData::getData() {
        return mInfo;
    }
}
