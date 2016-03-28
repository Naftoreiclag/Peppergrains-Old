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

#include "SandboxGameLayer.hpp"

#include <cassert>
#include <iostream>
#include <sstream>

#include "glm/gtx/string_cast.hpp"
#include "SDL2/SDL.h"

#include "PointLightModel.hpp"
#include "GrassModel.hpp"

namespace pgg
{

SandboxGameLayer::SandboxGameLayer(uint32_t width, uint32_t height)
: mScreenWidth(width)
, mScreenHeight(height) {
}

SandboxGameLayer::~SandboxGameLayer()
{
}

void SandboxGameLayer::makeLightVao() {
    ResourceManager* resman = ResourceManager::getSingleton();
    mDLightShaderProg = resman->findShaderProgram("DLight.shaderProgram");
    mDLightShaderProg->grab();
    const std::vector<ShaderProgramResource::Control>& sampler2DControls = mDLightShaderProg->getUniformSampler2Ds();
    for(std::vector<ShaderProgramResource::Control>::const_iterator iter = sampler2DControls.begin(); iter != sampler2DControls.end(); ++ iter) {
        const ShaderProgramResource::Control& entry = *iter;
        
        if(entry.name == "diffuse") {
            mDLightDiff = entry.handle;
        }
        else if(entry.name == "normal") {
            mDLightNorm = entry.handle;
        }
    }
    
    
    GLfloat vertices[] = {
        -1.f, -1.f,
         1.f, -1.f,
        -1.f,  1.f,
         1.f,  1.f
    };
    GLuint indices[] = {
        2, 0, 3,
        3, 0, 1,
    };
    
    glGenBuffers(1, &mDLightVbo);
    glBindBuffer(GL_ARRAY_BUFFER, mDLightVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &mDLightIbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mDLightIbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
    glGenVertexArrays(1, &mDLightVao);
    glBindVertexArray(mDLightVao);

    glBindBuffer(GL_ARRAY_BUFFER, mDLightVbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mDLightIbo);

    glEnableVertexAttribArray(mScreenShader.shaderProg->getPosAttrib());
    glVertexAttribPointer(mScreenShader.shaderProg->getPosAttrib(), 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (void*) (0 * sizeof(GLfloat)));

    glBindVertexArray(0);
}

void SandboxGameLayer::makeScreenShader() {
    ResourceManager* resman = ResourceManager::getSingleton();
    
    {
        mScreenShader.shaderProg = resman->findShaderProgram("GBuffer.shaderProgram");
        mScreenShader.shaderProg->grab();
        const std::vector<ShaderProgramResource::Control>& sampler2DControls = mScreenShader.shaderProg->getUniformSampler2Ds();
        for(std::vector<ShaderProgramResource::Control>::const_iterator iter = sampler2DControls.begin(); iter != sampler2DControls.end(); ++ iter) {
            const ShaderProgramResource::Control& entry = *iter;
            
            if(entry.name == "diffuse") {
                mScreenShader.diffuseHandle = entry.handle;
            }
            else if(entry.name == "normal") {
                mScreenShader.normalHandle = entry.handle;
            }
            else if(entry.name == "depth") {
                mScreenShader.depthHandle = entry.handle;
            }
            else if(entry.name == "bright") {
                mScreenShader.brightHandle = entry.handle;
            }
            else if(entry.name == "sunDepth") {
                mScreenShader.sunDepthHandle = entry.handle;
            }
        }
        const std::vector<ShaderProgramResource::Control>& vec3Controls = mScreenShader.shaderProg->getUniformVec3s();
        for(std::vector<ShaderProgramResource::Control>::const_iterator iter = vec3Controls.begin(); iter != vec3Controls.end(); ++ iter) {
            const ShaderProgramResource::Control& entry = *iter;
            
            if(entry.name == "sunDirection") {
                mScreenShader.sunDirectionHandle = entry.handle;
            }
        }
        
        assert(mScreenShader.shaderProg->needsInvViewProjMatrix() && "G-buffer shader does not accept inverse view projection matrix");
        assert(mScreenShader.shaderProg->needsSunViewProjMatrix() && "G-buffer shader does not accept sun view projection matrix");
    }
    {
        mDebugScreenShader.shaderProg = resman->findShaderProgram("GBufferDebug.shaderProgram");
        mDebugScreenShader.shaderProg->grab();
        const std::vector<ShaderProgramResource::Control>& sampler2DControls = mDebugScreenShader.shaderProg->getUniformSampler2Ds();
        for(std::vector<ShaderProgramResource::Control>::const_iterator iter = sampler2DControls.begin(); iter != sampler2DControls.end(); ++ iter) {
            const ShaderProgramResource::Control& entry = *iter;
            
            if(entry.name == "diffuse") {
                mDebugScreenShader.diffuseHandle = entry.handle;
            }
            else if(entry.name == "normal") {
                mDebugScreenShader.normalHandle = entry.handle;
            }
            else if(entry.name == "depth") {
                mDebugScreenShader.depthHandle = entry.handle;
            }
            else if(entry.name == "bright") {
                mDebugScreenShader.brightHandle = entry.handle;
            }
        }
        const std::vector<ShaderProgramResource::Control>& vec4Controls = mDebugScreenShader.shaderProg->getUniformVec4s();
        for(std::vector<ShaderProgramResource::Control>::const_iterator iter = vec4Controls.begin(); iter != vec4Controls.end(); ++ iter) {
            const ShaderProgramResource::Control& entry = *iter;
            
            if(entry.name == "showWhat") {
                mDebugScreenShader.viewHandle = entry.handle;
            }
        }
        
        assert(mDebugScreenShader.shaderProg->needsInvViewProjMatrix() && "Debug G-buffer shader does not accept inverse view projection matrix");
    }
    {
        mDebugFillScreenShader.shaderProg = resman->findShaderProgram("GBufferDebugFill.shaderProgram");
        mDebugFillScreenShader.shaderProg->grab();
    }
    
    
    // Fullscreen quad
    {
        GLfloat vertices[] = {
            -1.f, -1.f, 0.f, 0.f,
             1.f, -1.f, 1.f, 0.f,
            -1.f,  1.f, 0.f, 1.f,
             1.f,  1.f, 1.f, 1.f
        };
        GLuint indices[] = {
            2, 0, 3,
            3, 0, 1,
        };
        
        glGenBuffers(1, &mFullscreenVbo);
        glBindBuffer(GL_ARRAY_BUFFER, mFullscreenVbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glGenBuffers(1, &mFullscreenIbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mFullscreenIbo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        
        glGenVertexArrays(1, &mFullscreenVao);
        glBindVertexArray(mFullscreenVao);

        glBindBuffer(GL_ARRAY_BUFFER, mFullscreenVbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mFullscreenIbo);

        glEnableVertexAttribArray(mScreenShader.shaderProg->getPosAttrib());
        glVertexAttribPointer(mScreenShader.shaderProg->getPosAttrib(), 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*) (0 * sizeof(GLfloat)));
        glEnableVertexAttribArray(mScreenShader.shaderProg->getUVAttrib());
        glVertexAttribPointer(mScreenShader.shaderProg->getUVAttrib(), 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*) (2 * sizeof(GLfloat)));
        
        glEnableVertexAttribArray(mDebugScreenShader.shaderProg->getPosAttrib());
        glVertexAttribPointer(mDebugScreenShader.shaderProg->getPosAttrib(), 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*) (0 * sizeof(GLfloat)));
        glEnableVertexAttribArray(mDebugScreenShader.shaderProg->getUVAttrib());
        glVertexAttribPointer(mDebugScreenShader.shaderProg->getUVAttrib(), 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*) (2 * sizeof(GLfloat)));

        glBindVertexArray(0);
    }
}

void SandboxGameLayer::makeGBuffer() {
    // Create renderbuffer/textures for deferred shading
    {
        // Diffuse mapping
        glGenTextures(1, &mGBuff.diffuseTexture);
        glBindTexture(GL_TEXTURE_2D, mGBuff.diffuseTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, mScreenWidth, mScreenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glBindTexture(GL_TEXTURE_2D, 0);
        
        // Normal mapping
        glGenTextures(1, &mGBuff.normalTexture);
        glBindTexture(GL_TEXTURE_2D, mGBuff.normalTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, mScreenWidth, mScreenHeight, 0, GL_RGB, GL_FLOAT, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glBindTexture(GL_TEXTURE_2D, 0);
        
        // Bright mapping
        glGenTextures(1, &mGBuff.brightTexture);
        glBindTexture(GL_TEXTURE_2D, mGBuff.brightTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, mScreenWidth, mScreenHeight, 0, GL_RGB, GL_FLOAT, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glBindTexture(GL_TEXTURE_2D, 0);
        
        // DepthStencil mapping
        glGenTextures(1, &mGBuff.depthStencilTexture);
        glBindTexture(GL_TEXTURE_2D, mGBuff.depthStencilTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, mScreenWidth, mScreenHeight, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    
    // Create framebuffers
    {
        glGenFramebuffers(1, &mGBuff.gFramebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, mGBuff.gFramebuffer);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mGBuff.diffuseTexture, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, mGBuff.normalTexture, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, mGBuff.depthStencilTexture, 0);
        
        GLuint colorAttachments[] = {
            GL_COLOR_ATTACHMENT0,
            GL_COLOR_ATTACHMENT1
        };
        glDrawBuffers(2, colorAttachments);
        
        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE) {
            std::cout << "G Complete" << std::endl;
        }
        else {
            std::cout << "G Incomplete" << std::endl;
        }
        
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    {
        glGenFramebuffers(1, &mGBuff.bFramebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, mGBuff.bFramebuffer);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mGBuff.brightTexture, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, mGBuff.depthStencilTexture, 0);
        
        GLuint colorAttachments[] = {
            GL_COLOR_ATTACHMENT0
        };
        glDrawBuffers(1, colorAttachments);
        
        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE) {
            std::cout << "B Complete" << std::endl;
        }
        else {
            std::cout << "B Incomplete" << std::endl;
        }
        
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}

void SandboxGameLayer::makeSun() {
    mSunTextureWidth = 1024;
    
    // DepthStencil mapping
    glGenTextures(1, &mSunDepthTexture);
    glBindTexture(GL_TEXTURE_2D, mSunDepthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, mSunTextureWidth, mSunTextureWidth, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
    // TODO: change this
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);
    
    glGenFramebuffers(1, &mSunFrameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, mSunFrameBuffer);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, mSunDepthTexture, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "Sun Complete" << std::endl;
    }
    else {
        std::cout << "Sun Incomplete" << std::endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
}

// Lifecycle
void SandboxGameLayer::onBegin() {

    SDL_SetRelativeMouseMode(SDL_TRUE);
    
    glm::mat4 projMatOverlay = glm::ortho(0.f, (float) 1.f, 0.f, (float) 1.f);
    
    glm::vec4 test(0.0f, 0.0f, 0.0f, 1.0f);
    test = projMatOverlay * test;
    
    std::cout << glm::to_string(test) << std::endl;

    ResourceManager* resman = ResourceManager::getSingleton();
    
    makeScreenShader();
    makeGBuffer();
    //makeLightVao();
    makeSun();
    
    //testTerrain = new TerrainModel();
    //testTerrain->grab();
    
    mSunDir = glm::normalize(glm::vec3(-1.f, -1.f, -1.f));

    rootNode = new SceneNode();
    friendNodeX = new SceneNode();
    friendNodeY = new SceneNode();
    friendNodeZ = new SceneNode();
    testPlaneNode = new SceneNode();
    testGrassNode = new SceneNode();
    
    testPlaneNode->grabModel(resman->findModel("TestPlane.model"));
    //testPlaneNode->grabModel(new TerrainModel());
    rootNode->addChild(testPlaneNode);
    
    testGrassNode->grabModel(new GrassModel());
    rootNode->addChild(testGrassNode);
    
    rainstormFont = resman->findFont("Rainstorm.font");
    rainstormFont->grab();

    fpsCounter = new TextModel(rainstormFont, "FPS: Calculating...");
    fpsCounter->grab();

    friendNodeX->grabModel(resman->findModel("Door.model"));
    friendNodeY->grabModel(resman->findModel("NormalMapTestCube.model"));
    friendNodeZ->grabModel(resman->findModel("JellySmoothTorus.model"));

    
    rootNode->addChild(friendNodeX);
    rootNode->addChild(friendNodeY);
    rootNode->addChild(friendNodeZ);
    
    friendNodeX->move(glm::vec3(-3.f, 0.f, 0.f));
    friendNodeY->move(glm::vec3(0.f, 3.f, 0.f));
    friendNodeZ->move(glm::vec3(2.f, 2.f, 0.f));
    
    mCamRollNode = new SceneNode();
    mCamPitchNode = new SceneNode();
    mCamYawNode = new SceneNode();
    mCamLocNode = new SceneNode();
    mCamLocNode->move(glm::vec3(2.f, 4.f, 3.f));
    
    rootNode->addChild(mCamLocNode);
    mCamLocNode->addChild(mCamYawNode);
    mCamYawNode->addChild(mCamPitchNode);
    mCamPitchNode->addChild(mCamRollNode);

    mAxesModel = new AxesModel();
    mAxesModel->grab();
    
    iago = new SceneNode();
    iago->grabModel(new PointLightModel(glm::vec3(1.0f, 1.0f, 1.0f), 0.25f));
    rootNode->addChild(iago);

    fps = 0.f;
    mIago = 0.f;
    fpsWeight = 0.85f;
    
    mDebugWireframe = false;

    oneSecondTimer = 0.f;
}
void SandboxGameLayer::onEnd() {
    
    glDeleteBuffers(1, &mFullscreenIbo);
    glDeleteBuffers(1, &mFullscreenVbo);
    
    glDeleteTextures(1, &mGBuff.diffuseTexture);
    glDeleteTextures(1, &mGBuff.normalTexture);
    glDeleteTextures(1, &mGBuff.depthStencilTexture);
    glDeleteTextures(1, &mGBuff.brightTexture);
    
    glDeleteTextures(1, &mSunDepthTexture);
    glDeleteFramebuffers(1, &mSunFrameBuffer);

    mScreenShader.shaderProg->drop();
    mDebugScreenShader.shaderProg->drop();

    glDeleteVertexArrays(1, &mFullscreenVao);
    
    fpsCounter->drop();
    
    mAxesModel->drop();

    friendNodeX->dropModel();
    friendNodeY->dropModel();
    friendNodeZ->dropModel();
    
    testPlaneNode->dropModel();
    
    rainstormFont->drop();
    
    //testTerrain->drop();
    
    glDeleteFramebuffers(1, &mGBuff.gFramebuffer);
    glDeleteFramebuffers(1, &mGBuff.bFramebuffer);
}

// Ticks
void SandboxGameLayer::onTick(float tpf, const Uint8* keyStates) {
    
    glm::vec3 movement;
    if(keyStates[SDL_GetScancodeFromKey(SDLK_w)]) {
        movement.z -= 1.0;
    }
    if(keyStates[SDL_GetScancodeFromKey(SDLK_a)]) {
        movement.x -= 1.0;
    }
    if(keyStates[SDL_GetScancodeFromKey(SDLK_s)]) {
        movement.z += 1.0;
    }
    if(keyStates[SDL_GetScancodeFromKey(SDLK_d)]) {
        movement.x += 1.0;
    }
    if(movement != glm::vec3(0.f)) {
        glm::normalize(movement);
        if(keyStates[SDL_GetScancodeFromKey(SDLK_LSHIFT)]) {
            movement *= 10.f;
        }
        
        movement = glm::vec3(mCamRollNode->calcWorldTransform() * glm::vec4(movement, 0.f) * tpf);
        mCamLocNode->move(movement);
    }

    
    glm::mat4 viewMat = glm::inverse(mCamRollNode->calcWorldTransform());
    glm::mat4 projMat = glm::perspective(glm::radians(90.f), ((float) mScreenWidth) / ((float) mScreenHeight), 0.1f, 500.f);

    glm::mat4 viewMatOverlay;
    glm::mat4 projMatOverlay = glm::ortho(0.f, (float) mScreenWidth, 0.f, (float) mScreenHeight);
    glm::mat4 testMM;
    
    mIago += tpf;
    
    iago->setLocalTranslation(glm::vec3(0.f, 1.5f + (glm::sin(mIago) * 1.5f), 3.f));
    
    friendNodeY->rotate(glm::vec3(0.0f, 1.0f, 0.0f), tpf);
    friendNodeZ->rotate(glm::vec3(0.0f, 0.0f, 1.0f), tpf);
    
    if(keyStates[SDL_GetScancodeFromKey(SDLK_q)]) {
        mSunDir = glm::vec3(mCamRollNode->calcWorldTransform() * glm::vec4(0.f, 0.f, -1.f, 0.f));
    }
    
    
    glm::vec4 debugShow;
    if(keyStates[SDL_GetScancodeFromKey(SDLK_1)]) {
        debugShow.x = 1.f;
    }
    if(keyStates[SDL_GetScancodeFromKey(SDLK_2)]) {
        debugShow.y = 1.f;
    }
    if(keyStates[SDL_GetScancodeFromKey(SDLK_3)]) {
        debugShow.z = 1.f;
    }
    if(keyStates[SDL_GetScancodeFromKey(SDLK_4)]) {
        debugShow.w = 1.f;
    }
    if(keyStates[SDL_GetScancodeFromKey(SDLK_5)]) {
        mDebugWireframe = true;
    }
    if(keyStates[SDL_GetScancodeFromKey(SDLK_6)]) {
        mDebugWireframe = false;
    }
    
    //mCamRollNode->calcWorldTranslation(mSunPos);
    //mSunDir = glm::vec3(glm::sin(mIago), -1.f, glm::cos(mIago));
    //mSunDir = glm::normalize(mSunDir);
    
    /*
    mSunDir = glm::vec3(-1.f, -1.f, -1.f);
    mSunDir = glm::normalize(mSunDir);
    */
    
    mSunViewMatr = glm::lookAt(mSunPos - mSunDir, mSunPos, glm::vec3(0.f, 1.f, 0.f));
    mSunProjMatr = glm::ortho(-10.f, 10.f, -10.f, 10.f, -10.f, 10.f);
    
    // Sun? buffer
    glViewport(0, 0, mSunTextureWidth, mSunTextureWidth);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mSunFrameBuffer);
    glDepthMask(GL_TRUE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
    glDisable(GL_BLEND);
    glClear(GL_DEPTH_BUFFER_BIT);
    
    Model::RenderPassConfiguration sunRPC(Model::RenderPassType::SHADOW);
    sunRPC.viewMat = mSunViewMatr;
    sunRPC.projMat = mSunProjMatr;
    rootNode->render(sunRPC);
    
    // G-buffer render
    glViewport(0, 0, mScreenWidth, mScreenHeight);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mGBuff.gFramebuffer);
    glClearColor(0.f, 0.f, 0.f, 1.f);
    glDepthMask(GL_TRUE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDisable(GL_STENCIL_TEST);
    glEnable(GL_CULL_FACE);
    glDisable(GL_BLEND);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    if(mDebugWireframe) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    
    Model::RenderPassConfiguration rootNodeRPC(Model::RenderPassType::GEOMETRY);
    rootNodeRPC.viewMat = viewMat;
    rootNodeRPC.projMat = projMat;
    rootNode->render(rootNodeRPC);
    
    // Brightness Render
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mGBuff.bFramebuffer);
    
    // Clear brightness
    glClearColor(0.f, 0.0f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    // Do not write to the depth buffer
    glDepthMask(GL_FALSE);
    
    // Use depth testing
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    
    Model::RenderPassConfiguration brightRPC(Model::RenderPassType::BRIGHT);
    brightRPC.viewMat = viewMat;
    brightRPC.projMat = projMat;
    brightRPC.depthStencilTexture = mGBuff.depthStencilTexture;
    brightRPC.normalTexture = mGBuff.normalTexture;
    rootNode->render(brightRPC);
    
    // Stencil, culling, and blending were modified, reset to default values
    glDisable(GL_STENCIL_TEST);
    glEnable(GL_CULL_FACE);    
    glDisable(GL_BLEND);
    glCullFace(GL_BACK);
    
    // Screen render
    glViewport(0, 0, mScreenWidth, mScreenHeight);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glDepthMask(GL_FALSE);
    glDisable(GL_DEPTH_TEST);
    glDepthFunc(GL_EQUAL);
    glEnable(GL_CULL_FACE);
    glDisable(GL_BLEND);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    
    if(debugShow != glm::vec4(0.f)) {
        glUseProgram(mDebugScreenShader.shaderProg->getHandle());
        
        glm::mat4 invViewProjMat = glm::inverse(projMat * viewMat);
        glUniformMatrix4fv(mDebugScreenShader.shaderProg->getInvViewProjMatrixUnif(), 1, GL_FALSE, glm::value_ptr(invViewProjMat));
        glUniform4fv(mDebugScreenShader.viewHandle, 1, glm::value_ptr(debugShow));
        
        glActiveTexture(GL_TEXTURE0 + 0);
        glBindTexture(GL_TEXTURE_2D, mGBuff.diffuseTexture);
        glUniform1i(mDebugScreenShader.diffuseHandle, 0);
        
        glActiveTexture(GL_TEXTURE0 + 1);
        glBindTexture(GL_TEXTURE_2D, mGBuff.normalTexture);
        glUniform1i(mDebugScreenShader.normalHandle, 1);
        
        glActiveTexture(GL_TEXTURE0 + 2);
        glBindTexture(GL_TEXTURE_2D, mGBuff.brightTexture);
        glUniform1i(mDebugScreenShader.brightHandle, 2);
        
        glActiveTexture(GL_TEXTURE0 + 3);
        glBindTexture(GL_TEXTURE_2D, mGBuff.depthStencilTexture);
        glUniform1i(mDebugScreenShader.depthHandle, 3);
        
        glBindVertexArray(mFullscreenVao);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        
        glBindVertexArray(0);
        
        glUseProgram(0);
    } else {
        glUseProgram(mScreenShader.shaderProg->getHandle());
     
        glm::mat4 invViewProjMat = glm::inverse(projMat * viewMat);
        glUniformMatrix4fv(mScreenShader.shaderProg->getInvViewProjMatrixUnif(), 1, GL_FALSE, glm::value_ptr(invViewProjMat));
        
        glm::mat4 sunViewProjMat = mSunProjMatr * mSunViewMatr;
        glUniformMatrix4fv(mScreenShader.shaderProg->getSunViewProjMatrixUnif(), 1, GL_FALSE, glm::value_ptr(sunViewProjMat));
        glUniform3fv(mScreenShader.sunDirectionHandle, 1, glm::value_ptr(mSunDir));
        
        glActiveTexture(GL_TEXTURE0 + 0);
        glBindTexture(GL_TEXTURE_2D, mGBuff.diffuseTexture);
        glUniform1i(mScreenShader.diffuseHandle, 0);
        
        glActiveTexture(GL_TEXTURE0 + 1);
        glBindTexture(GL_TEXTURE_2D, mGBuff.normalTexture);
        glUniform1i(mScreenShader.normalHandle, 1);
        
        glActiveTexture(GL_TEXTURE0 + 2);
        glBindTexture(GL_TEXTURE_2D, mGBuff.depthStencilTexture);
        glUniform1i(mScreenShader.depthHandle, 2);
        
        glActiveTexture(GL_TEXTURE0 + 3);
        glBindTexture(GL_TEXTURE_2D, mGBuff.brightTexture);
        glUniform1i(mScreenShader.brightHandle, 3);
        
        glActiveTexture(GL_TEXTURE0 + 4);
        glBindTexture(GL_TEXTURE_2D, mSunDepthTexture);
        glUniform1i(mScreenShader.sunDepthHandle, 4);
        
        glBindVertexArray(mFullscreenVao);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        
        glBindVertexArray(0);
        
        glUseProgram(0);
    }
    
    // Skybox render
    /*
    glViewport(0, 0, mScreenWidth, mScreenHeight);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glDepthMask(GL_FALSE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_CULL_FACE);
    glDisable(GL_BLEND);
    
    glUseProgram(mDebugFillScreenShader.shaderProg->getHandle());
    glBindVertexArray(mFullscreenVao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    glUseProgram(0);
    */
    
    
    
    if(tpf > 0) {
        float fpsNew = 1 / tpf;
        fps = (fps * fpsWeight) + (fpsNew * (1.f - fpsWeight));
    }

    oneSecondTimer += tpf;
    if(oneSecondTimer > 1.f) {
        oneSecondTimer -= 1.f;

        fpsCounter->drop();

        std::stringstream ss;
        ss << "FPS: ";
        ss << (uint32_t) fps;

        fpsCounter = new TextModel(rainstormFont, ss.str());
        fpsCounter->grab();
    }
    
    
    Model::RenderPassConfiguration fpsRPC(Model::RenderPassType::SCREEN);
    fpsRPC.viewMat = viewMatOverlay;
    fpsRPC.projMat = projMatOverlay;
    fpsCounter->render(fpsRPC, testMM);
}

bool SandboxGameLayer::onMouseMove(const SDL_MouseMotionEvent& event) {
    float x = event.x;
    float y = event.y;
    float dx = event.xrel;
    float dy = event.yrel;
    
    mCamYawNode->rotateYaw(-dx * 0.003f);
    mCamPitchNode->rotatePitch(-dy * 0.003f);
    
    return true;
}

bool SandboxGameLayer::onWindowSizeUpdate(const SDL_WindowEvent& event) {
    std::cout << event.data1 << ", " << event.data2 << std::endl;
    
    return true;
}
}

