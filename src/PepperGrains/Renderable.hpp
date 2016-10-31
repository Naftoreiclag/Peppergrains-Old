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

#ifndef PGG_RENDERABLE_HPP
#define PGG_RENDERABLE_HPP

#include "OpenGLStuff.hpp"
#include "HardValueStuff.hpp"

namespace pgg {
class Renderable {
public:
    // Might someday have different configs for different renderables
    struct RenderPass {
        enum Type {
            GEOMETRY,
            SHADOW,
            LOCAL_LIGHTS,
            GLOBAL_LIGHTS,
            SMAC_GEOMETRY,
            SSIPG,
            SCREEN
        };
        
        glm::mat4 viewMat;
        glm::mat4 projMat;
        
        glm::vec3 camPos;
        glm::vec3 camDir;
        
        float nearPlane;
        float farPlane;
        
        glm::vec2 screenSize;
        glm::vec2 invScreenSize;
        
        void setScreenSize(uint32_t width, uint32_t height);
        
        float cascadeBorders[PGG_NUM_SUN_CASCADES + 1];
        
        GLuint framebuffer;
        GLuint diffuseTexture;
        GLuint normalTexture;
        GLuint brightTexture;
        GLuint depthStencilTexture;
        
        GLuint sunDepthTexture[PGG_NUM_SUN_CASCADES];
        glm::mat4 sunViewProjMatr[PGG_NUM_SUN_CASCADES];
        
        Type type;
        
        RenderPass(RenderPass::Type renderPassType);
        ~RenderPass();
        
        bool availableFustrumAABB;
        glm::vec3 minBB;
        glm::vec3 maxBB;
    
        void calculateFustrumAABB();
    };
    
    virtual void render(const Renderable::RenderPass& rendPass) = 0;
};
}

#endif // PGG_RENDERABLE_HPP
