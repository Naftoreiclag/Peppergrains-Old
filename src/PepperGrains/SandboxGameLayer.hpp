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

#include "AxesModel.hpp"
#include "SceneNode.hpp"
#include "ResourceManager.hpp"
#include "TextModel.hpp"

namespace pgg {

class SandboxGameLayer : public GameLayer {
public:
    SandboxGameLayer();
    ~SandboxGameLayer();
    
    SceneNode* rootNode;
    SceneNode* friendNodeX;
    SceneNode* friendNodeY;
    SceneNode* friendNodeZ;
    
    SceneNode* testPlaneNode;
    
    AxesModel* mAxesModel;
    
    ShaderProgramResource* mGBufferShaderProg;
    TextureResource* mTestTexture;
    GLuint mDiffuseHandle;
    GLuint mNormalHandle;
    GLuint mPositionHandle;
    
    GLuint mVertexArrayObject;
    GLuint mVertexBufferObject;
    GLuint mIndexBufferObject;
    
    GLuint mFramebuffer;
    GLuint mDiffuseTexture;
    GLuint mNormalTexture;
    GLuint mPositionTexture;
    GLuint mDepthStencilRenderBuffer;
    
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
    
    bool onWindowSizeUpdate(const SDL_WindowEvent& event);
};

}

#endif // SANDBOXGAMELAYER_HPP
