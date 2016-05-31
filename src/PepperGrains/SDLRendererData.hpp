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

#ifndef PGG_SDLRENDERERDATA_HPP
#define PGG_SDLRENDERERDATA_HPP

#include <string>

#include "SDL2/SDL.h"

namespace pgg
{

class SDLRendererData {
public:
    struct SDLInfo {
        std::string name;
        bool softwareFallback;
        bool hardwareAccelerated;
        bool supportTextureRender;
    };
private:
    SDLInfo mInfo;
public:
    static SDLRendererData* getSingleton();
public:
    SDLRendererData();
    ~SDLRendererData();

    void loadData(SDL_Renderer* renderer);
    
    const SDLInfo& getData();
};

}

#endif // PGG_SDLRENDERERDATA_HPP
