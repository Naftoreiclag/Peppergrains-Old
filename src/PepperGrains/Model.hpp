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

#ifndef PGG_MODEL_HPP
#define PGG_MODEL_HPP

#include "ReferenceCounted.hpp"
#include "OpenGLStuff.hpp"
#include "HardValueStuff.hpp"

namespace pgg {

// Virtual inheritance to avoid diamond conflict with ModelResource
class Model : virtual public ReferenceCounted {
public:
    Model();
    virtual ~Model();
    
    enum RenderPassType {
        GEOMETRY,
        SHADOW,
        LOCAL_LIGHTS,
        GLOBAL_LIGHTS,
        SCREEN
    };
    
    // Might someday have different configs for different renderables
    struct RenderPass {
        glm::mat4 viewMat;
        glm::mat4 projMat;
        
        glm::vec3 camPos;
        
        float cascadeBorders[PGG_NUM_SUN_CASCADES + 1];
        
        GLuint framebuffer;
        GLuint diffuseTexture;
        GLuint normalTexture;
        GLuint brightTexture;
        GLuint depthStencilTexture;
        
        GLuint sunDepthTexture[PGG_NUM_SUN_CASCADES];
        glm::mat4 sunViewProjMatr[PGG_NUM_SUN_CASCADES];
        
        RenderPassType type;
        
        RenderPass(RenderPassType renderPassType);
        ~RenderPass();
        
        bool availableFustrumAABB;
        glm::vec3 minBB;
        glm::vec3 maxBB;
    
        void calculateFustrumAABB();
    };

    virtual void render(const RenderPass& rendPass, const glm::mat4& modelMat) = 0;
};

}

#endif // PGG_MODEL_HPP
