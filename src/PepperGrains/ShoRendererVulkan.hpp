/*
   Copyright 2017 James Fong

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

#ifndef PGG_SHORENDERERVULKAN_HPP
#define PGG_SHORENDERERVULKAN_HPP

#ifdef PGG_VULKAN

#include <stdint.h>

#include <GraphicsApiLibrary.hpp>

#include "ShaderProgramResource.hpp"
#include "Scenegraph.hpp"
#include "Camera.hpp"

namespace pgg {

class ShoRenderer {
public:
    ShoRenderer(uint32_t screenWidth, uint32_t screenHeight);
    ~ShoRenderer();
    
    Camera mCamera;
    Scenegraph* mScenegraph;

    void resize(uint32_t width, uint32_t height);
    void renderFrame();
};

}

#endif // PGG_VULKAN

#endif // PGG_SHORENDERERVULKAN_HPP
