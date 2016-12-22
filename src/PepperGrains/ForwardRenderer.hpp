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

#ifndef PGG_FORWARDRENDERER_HPP
#define PGG_FORWARDRENDERER_HPP

#include <stdint.h>

#include "OpenGLStuff.hpp"
#include "ShaderProgramResource.hpp"
#include "Renderable.hpp"

namespace pgg {

class ForwardRenderer {
public:
    ForwardRenderer(uint32_t screenWidth, uint32_t screenHeight, Renderable* renderable = nullptr);
    ~ForwardRenderer();
private:
    uint32_t mScreenWidth;
    uint32_t mScreenHeight;

    struct GBuffer {
        GLuint framebuffer;
        
        GLuint forwardTexture;
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
    
public:
    void load();
    void unload();
    
    void renderFrame();
    
    struct Camera {
        float fov;
        float aspect;
        float nearDepth;
        float farDepth;
        
        glm::mat4 viewMat;
        glm::mat4 projMat;
        glm::vec3 location;
    };
    
    Renderable* mRenderable;
    Camera mCamera;
};

}

#endif // PGG_FORWARDRENDERER_HPP
