/*
   Copyright 2016-2017 James Fong

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

#ifndef PGG_PEPPERGRAINSENGINE_HPP
#define PGG_PEPPERGRAINSENGINE_HPP

#include <stdint.h>

#include <WindowInputSystemLibrary.hpp>

#include "GamelayerMachine.hpp"
#include "SoundEndpoint.hpp"

namespace pgg {
namespace Engine {
    
    extern const char* mName;
    
    extern Sound::Endpoint mSoundEndpoint;
    extern GamelayerMachine mGamelayerMachine;
    
    void quit();
    
    #ifdef PGG_SDL
    SDL_Window* getSdlWindow();
    #endif
    
    #ifdef PGG_GLFW
    GLFWwindow* getGlfwWindow();
    #endif
    
} // Engine
} // pgg

#endif // PGG_PEPPERGRAINSENGINE_HPP
