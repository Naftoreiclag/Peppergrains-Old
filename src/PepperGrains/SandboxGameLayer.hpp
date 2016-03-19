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

#ifndef SANDBOXGAMELAYER_HPP
#define SANDBOXGAMELAYER_HPP

#include "GameLayer.hpp" // Base class: pgg::GameLayer

#include "SDL2/SDL.h"

#include "AxesModel.hpp"
#include "SceneNode.hpp"
#include "ResourceManager.hpp"
#include "TerrainModel.hpp"
#include "TextModel.hpp"

namespace pgg {

class SandboxGameLayer : public GameLayer {
private:
    struct ScreenShader {
        ShaderProgramResource* shaderProg;
        GLuint diffuseHandle;
        GLuint normalHandle;
        GLuint depthHandle;
        GLuint sunDepthHandle;
        GLuint sunDirectionHandle;
    };
    
    ScreenShader mScreenShader;
    
    struct DebugScreenShader {
        ShaderProgramResource* shaderProg;
        GLuint diffuseHandle;
        GLuint normalHandle;
        GLuint depthHandle;
        GLuint viewHandle;
    };
    
    DebugScreenShader mDebugScreenShader;
    
    uint32_t mScreenWidth;
    uint32_t mScreenHeight;
    
    void makeScreenShader();
    void makeGBuffer();
    void makeLightVao();
    void makeSun();
public:
    SandboxGameLayer(uint32_t width, uint32_t height);
    ~SandboxGameLayer();
    
    SceneNode* rootNode;
    SceneNode* friendNodeX;
    SceneNode* friendNodeY;
    SceneNode* friendNodeZ;
    SceneNode* iago;
    
    SceneNode* mCamLocNode;
    SceneNode* mCamYawNode;
    SceneNode* mCamPitchNode;
    SceneNode* mCamRollNode;
    
    SceneNode* testPlaneNode;
    SceneNode* testGrassNode;
    
    AxesModel* mAxesModel;
    
    GLuint mFullscreenVao;
    GLuint mFullscreenVbo;
    GLuint mFullscreenIbo;
    
    ShaderProgramResource* mDLightShaderProg;
    GLuint mDLightDiff;
    GLuint mDLightNorm;
    GLuint mDLightVao;
    GLuint mDLightVbo;
    GLuint mDLightIbo;
    
    uint32_t mSunTextureWidth;
    GLuint mSunFrameBuffer;
    GLuint mSunDepthTexture;
    glm::vec3 mSunDir;
    glm::vec3 mSunPos;
    glm::mat4 mSunProjMatr;
    glm::mat4 mSunViewMatr;
    
    float mIago;
    
    GLuint mGFramebuffer;
    GLuint mGDiffuseTexture;
    GLuint mGNormalTexture;
    GLuint mGDepthStencilTexture;
    
    FontResource* rainstormFont;
    TextModel* fpsCounter;
    TerrainModel* testTerrain;

    float fps;
    float fpsWeight;

    float oneSecondTimer;
    
    bool mDebugWireframe;
    
    // Lifecycle
    void onBegin();
    void onEnd();
    
    // Ticks
    void onTick(float tps, const Uint8* keyStates);
    bool onMouseMove(const SDL_MouseMotionEvent& event);
    
    bool onWindowSizeUpdate(const SDL_WindowEvent& event);
};

}

#endif // SANDBOXGAMELAYER_HPP
