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

#ifndef PGG_DEFERREDRENDERER_HPP
#define PGG_DEFERREDRENDERER_HPP

#include <stdint.h>

#include "OpenGLStuff.hpp"
#include "HardValueStuff.hpp"
#include "ReferenceCounted.hpp"
#include "SceneNode.hpp"
#include "ShaderProgramResource.hpp"
#include "SunLightModel.hpp"

namespace pgg {

class DeferredRenderer : public ReferenceCounted {
private:
    uint32_t mScreenWidth;
    uint32_t mScreenHeight;

    GLuint mFullscreenVao;
    GLuint mFullscreenVbo;
    GLuint mFullscreenIbo;

    struct GBuffer {
        GLuint gFramebuffer;
        
        GLuint diffuseTexture;
        GLuint normalTexture;
        GLuint brightTexture;
        GLuint depthStencilTexture;
    };
    GBuffer mGBuff;

    struct ScreenShader {
        ShaderProgramResource* shaderProg;
        
        GLuint diffuseHandle;
        GLuint brightHandle;
    };
    ScreenShader mScreenShader;
    
    struct DebugScreenShader {
        ShaderProgramResource* shaderProg;
        
        GLuint diffuseHandle;
        GLuint normalHandle;
        GLuint depthHandle;
        GLuint brightHandle;
        
        GLuint viewHandle;
    };
    DebugScreenShader mDebugScreenShader;
    
    struct SkyStencilShader {
        ShaderProgramResource* shaderProg;
    };
    SkyStencilShader mSkyStencilShader;
    
    struct DebugScreenFillShader {
        ShaderProgramResource* shaderProg;
        GLuint colorHandle;
    };
    DebugScreenFillShader mFillScreenShader;
    
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
    
    struct Sun {
        uint32_t shadowMapResolution;
        
        glm::vec3 direction;
        glm::vec3 location;
        
        GLuint framebuffers[PGG_NUM_SUN_CASCADES];
        GLuint depthTextures[PGG_NUM_SUN_CASCADES];
        
        glm::mat4 projectionMatrices[PGG_NUM_SUN_CASCADES];
        
        glm::mat4 viewMatrix;
        
        SunLightModel* sunModel;
    };
    Sun mSky;
    
public:
    DeferredRenderer(uint32_t width, uint32_t height);
    ~DeferredRenderer();
    
    void load();
    void unload();
    
    void renderFrame(SceneNode* mRootNode, glm::vec4 debugShow, bool wireframe);
    
    void setSunDirection(const glm::vec3& dirSunAiming);
    
    void setCameraProjection(float fov, float nearDepth, float farDepth);
    void setCameraViewMatrix(const glm::mat4& camViewMatrix);
    
    const glm::mat4& getCameraProjectionMatrix() const;
    const glm::mat4& getCameraViewMatrix() const;
    
    // void resizeScreen(uint32_t width, uint32_t height);
    
    const glm::vec3& getCameraLocation() const;
};

}

#endif // PGG_DEFERREDRENDERER_HPP