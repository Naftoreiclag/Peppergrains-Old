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

#ifndef PGG_SHORENDERER_HPP
#define PGG_SHORENDERER_HPP

#include <stdint.h>

#include "OpenGLStuff.hpp"
#include "ShaderProgramResource.hpp"
#include "Renderable.hpp"

namespace pgg {

// sho = Spherical Harmonic per-Object renderer
class ShoRenderer {
public:
    ShoRenderer(uint32_t screenWidth, uint32_t screenHeight);
    ~ShoRenderer();
private:
    Renderable* mRenderable;
    
    uint32_t mScreenWidth;
    uint32_t mScreenHeight;

    GLuint mTextureDepthStencil;
    GLuint mTextureShadowBias;
    GLuint mTextureSunlightIrradiance;
    GLuint mTextureForward;
    
    GLuint mFramebufferDepthPrepass;
    GLuint mFramebufferSunlightIrradiance;
    GLuint mFramebufferForward;

    uint32_t mSunlightShadowMapResolution;
    glm::vec3 mSunlightDirection;
    GLuint mSunlightFramebuffers[PGG_NUM_SUN_CASCADES];
    GLuint mSunlightDepthTextures[PGG_NUM_SUN_CASCADES];
    glm::mat4 mSunlightProjectionMatrices[PGG_NUM_SUN_CASCADES];
    glm::mat4 mSunlightViewMatrix;

    ShaderProgramResource* mSunlightIrradianceShaderProg;
    GLuint mPostProcessShaderHandle;
    
    ShaderProgramResource* mPostProcessShaderProg;
    GLuint mPostProcessShaderForwardHandle;
    
    GLuint mFullscreenVao;
    GLuint mFullscreenVbo;
    GLuint mFullscreenIbo;
    
    struct Camera {
        float fov;
        float aspect;
        float nearDepth;
        float farDepth;
        
        float cascadeBorders[PGG_NUM_SUN_CASCADES + 1];
        
        glm::mat4 viewMat;
        glm::mat4 projMat;
        glm::vec3 position;
    };
    Camera mCamera;
    
public:
    void load();
    void unload();
    
    void resize(uint32_t width, uint32_t height);
    
    void setRenderable(Renderable* renderable);
    
    void renderFrame();
    
    void addRenderable();
    void removeRenderable();
};

}

#endif // PGG_SHORENDERER_HPP
