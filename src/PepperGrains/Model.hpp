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
    struct RenderPassConfiguration {
        glm::mat4 viewMat;
        glm::mat4 projMat;
        
        glm::vec3 camPos;
        
        GLuint framebuffer;
        GLuint diffuseTexture;
        GLuint normalTexture;
        GLuint brightTexture;
        GLuint depthStencilTexture;
        GLuint sunDepthTexture;
        glm::mat4 sunViewProjMatr;
        
        RenderPassType type;
        
        RenderPassConfiguration(RenderPassType renderPassType);
        ~RenderPassConfiguration();
    };

    virtual void render(const RenderPassConfiguration& rendPass, const glm::mat4& modelMat) = 0;
};

}

#endif // PGG_MODEL_HPP
