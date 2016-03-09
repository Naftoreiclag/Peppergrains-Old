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
#include "TextModel.hpp"

namespace pgg {

class SandboxGameLayer : public GameLayer {
private:
    void makeGBuffer();
    void makeLightVao();
    void makeSun();
public:
    SandboxGameLayer();
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
    
    AxesModel* mAxesModel;
    
    ShaderProgramResource* mGBufferShaderProg;
    GLuint mDiffuseHandle;
    GLuint mNormalHandle;
    GLuint mDepthHandle;
    GLuint mSunDepthHandle;
    GLuint mSunDirectionHandle;
    //GLuint mBrightHandle;
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
    glm::mat4 mSunProjMatr;
    glm::mat4 mSunViewMatr;
    
    float mIago;
    
    GLuint mFramebuffer;
    GLuint mDiffuseTexture;
    GLuint mNormalTexture;
    GLuint mDepthStencilTexture;
    //GLuint mBrightTexture;
    
    FontResource* rainstormFont;
    TextModel* fpsCounter;

    float fps;
    float fpsWeight;

    float oneSecondTimer;
    
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
