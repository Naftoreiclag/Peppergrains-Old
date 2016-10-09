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

#ifndef PGG_SMACRENDERER_HPP
#define PGG_SMACRENDERER_HPP

#include <stdint.h>

#include "OpenGLStuff.hpp"
#include "ReferenceCounted.hpp"
#include "SceneNode.hpp"
#include "ShaderProgramResource.hpp"

namespace pgg {

// smac = Silly MAtcap Renderer
class SmacRenderer : public ReferenceCounted {
public:
    SmacRenderer(uint32_t screenWidth, uint32_t screenHeight);
    ~SmacRenderer();
private:
    uint32_t mScreenWidth;
    uint32_t mScreenHeight;
    
    SceneNode* mRootNode;

    struct GBuffer {
        GLuint framebuffer;
        
        GLuint forwardTexture;
        GLuint normalTexture;
        GLuint matpropTexture;
        GLuint depthStencilTexture;
    };
    GBuffer mGBuff;

    struct ScreenShader {
        ShaderProgramResource* shaderProg;
        
        GLuint forwardHandle;
        
    };
    ScreenShader mScreenShader;
    
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
    
    SceneNode* getRootNode();
    
    void renderFrame();
};

}

#endif // PGG_SMACRENDERER_HPP
