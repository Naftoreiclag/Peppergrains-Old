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

namespace pgg
{

SandboxGameLayer::SandboxGameLayer()
{
}

SandboxGameLayer::~SandboxGameLayer()
{
}

void SandboxGameLayer::makeLightVao() {
    ResourceManager* resman = ResourceManager::getSingleton();
    mDLightShaderProg = resman->findShaderProgram("DLight.shaderProgram");
    mDLightShaderProg->grab();
    const std::vector<ShaderProgramResource::Control>& sampler2DControls = mDLightShaderProg->getSampler2Ds();
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
    
    // Locate where to send the sampler
    {
        mScreenShader.shaderProg = resman->findShaderProgram("GBuffer.shaderProgram");
        mScreenShader.shaderProg->grab();
        const std::vector<ShaderProgramResource::Control>& sampler2DControls = mScreenShader.shaderProg->getSampler2Ds();
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
            else if(entry.name == "sunDepth") {
                mScreenShader.sunDepthHandle = entry.handle;
            }
            /*
            else if(entry.name == "bright") {
                mBrightHandle = entry.handle;
            }
            */
        }
        const std::vector<ShaderProgramResource::Control>& vec3Controls = mScreenShader.shaderProg->getVec3s();
        for(std::vector<ShaderProgramResource::Control>::const_iterator iter = vec3Controls.begin(); iter != vec3Controls.end(); ++ iter) {
            const ShaderProgramResource::Control& entry = *iter;
            
            if(entry.name == "sunDirection") {
                mScreenShader.sunDirectionHandle = entry.handle;
            }
        }
        
        assert(mScreenShader.shaderProg->needsInvViewProjMatrix() && "G-buffer shader does not accept inverse view projection matrix");
        assert(mScreenShader.shaderProg->needsSunViewProjMatrix() && "G-buffer shader does not accept sun view projection matrix");
    }
    // Locate where to send the sampler
    {
        mDebugScreenShader.shaderProg = resman->findShaderProgram("GBufferDebug.shaderProgram");
        mDebugScreenShader.shaderProg->grab();
        const std::vector<ShaderProgramResource::Control>& sampler2DControls = mDebugScreenShader.shaderProg->getSampler2Ds();
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
        }
        const std::vector<ShaderProgramResource::Control>& vec4Controls = mDebugScreenShader.shaderProg->getVec4s();
        for(std::vector<ShaderProgramResource::Control>::const_iterator iter = vec4Controls.begin(); iter != vec4Controls.end(); ++ iter) {
            const ShaderProgramResource::Control& entry = *iter;
            
            if(entry.name == "showWhat") {
                mDebugScreenShader.viewHandle = entry.handle;
            }
        }
        
        assert(mDebugScreenShader.shaderProg->needsInvViewProjMatrix() && "Debug G-buffer shader does not accept inverse view projection matrix");
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
    ResourceManager* resman = ResourceManager::getSingleton();
    
    // Create renderbuffer/textures for deferred shading
    {
        // Diffuse mapping
        glGenTextures(1, &mGDiffuseTexture);
        glBindTexture(GL_TEXTURE_2D, mGDiffuseTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1280, 720, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glBindTexture(GL_TEXTURE_2D, 0);
        
        // Normal mapping
        glGenTextures(1, &mGNormalTexture);
        glBindTexture(GL_TEXTURE_2D, mGNormalTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 1280, 720, 0, GL_RGB, GL_FLOAT, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glBindTexture(GL_TEXTURE_2D, 0);
        
        // Bright mapping
        /*
        glGenTextures(1, &mBrightHandle);
        glBindTexture(GL_TEXTURE_2D, mBrightHandle);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 1280, 720, 0, GL_RGB, GL_FLOAT, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glBindTexture(GL_TEXTURE_2D, 0);
        */
        
        // DepthStencil mapping
        glGenTextures(1, &mGDepthStencilTexture);
        glBindTexture(GL_TEXTURE_2D, mGDepthStencilTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, 1280, 720, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    
    // Create framebuffer
    {
        glGenFramebuffers(1, &mGFramebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, mGFramebuffer);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mGDiffuseTexture, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, mGNormalTexture, 0);
        //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, mBrightTexture, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, mGDepthStencilTexture, 0);
        
        GLuint colorAttachments[] = {
            GL_COLOR_ATTACHMENT0,
            GL_COLOR_ATTACHMENT1,
            GL_COLOR_ATTACHMENT2
            //GL_COLOR_ATTACHMENT3
        };
        glDrawBuffers(3, colorAttachments);
        
        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE) {
            std::cout << "G Complete" << std::endl;
        }
        else {
            std::cout << "G Incomplete" << std::endl;
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
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

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
    makeLightVao();
    makeSun();

    rootNode = new SceneNode();
    friendNodeX = new SceneNode();
    friendNodeY = new SceneNode();
    friendNodeZ = new SceneNode();
    testPlaneNode = new SceneNode();
    
    testPlaneNode->grabModel(resman->findModel("Hills.model"));
    rootNode->addChild(testPlaneNode);
    
    rainstormFont = resman->findFont("Rainstorm.font");
    rainstormFont->grab();

    fpsCounter = new TextModel(rainstormFont, "FPS: Calculating...");
    fpsCounter->grab();

    friendNodeX->grabModel(resman->findModel("JellySmoothTorus.model"));
    friendNodeY->grabModel(resman->findModel("NormalMapTestCube.model"));
    friendNodeZ->grabModel(resman->findModel("JellySmoothTorus.model"));

    rootNode->addChild(friendNodeX);
    rootNode->addChild(friendNodeY);
    rootNode->addChild(friendNodeZ);
    
    friendNodeX->move(glm::vec3(-2.f, 2.f, 0.f));
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
    //iago->grabModel(resman->findModel("Iago.model"));
    iago->move(glm::vec3(0.f, 3.5f, 0.f));
    rootNode->addChild(iago);

    fps = 0.f;
    mIago = 0.f;
    fpsWeight = 0.85f;

    oneSecondTimer = 0.f;
}
void SandboxGameLayer::onEnd() {
    
    glDeleteBuffers(1, &mFullscreenIbo);
    glDeleteBuffers(1, &mFullscreenVbo);
    
    glDeleteTextures(1, &mGDiffuseTexture);
    glDeleteTextures(1, &mGNormalTexture);
    glDeleteTextures(1, &mGDepthStencilTexture);
    //glDeleteTextures(1, &mBrightTexture);
    
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
    
    glDeleteFramebuffers(1, &mGFramebuffer);
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
        
        movement = glm::vec3(mCamRollNode->calcWorldTransform() * glm::vec4(movement, 0.f) * tpf);
        mCamLocNode->move(movement);
    }

    
    glm::mat4 viewMat = glm::inverse(mCamRollNode->calcWorldTransform());
    glm::mat4 projMat = glm::perspective(glm::radians(90.f), 1280.f / 720.f, 0.1f, 500.f);

    glm::mat4 viewMatOverlay;
    glm::mat4 projMatOverlay = glm::ortho(0.f, (float) 1280, 0.f, (float) 720);
    glm::mat4 testMM;
    
    mIago += tpf;
    
    iago->setLocalTranslation(glm::vec3(0.f, 0.2f + glm::sin(mIago), 3.f));
    
    friendNodeX->rotate(glm::vec3(1.0f, 0.0f, 0.0f), tpf);
    friendNodeY->rotate(glm::vec3(0.0f, 1.0f, 0.0f), tpf);
    friendNodeZ->rotate(glm::vec3(0.0f, 0.0f, 1.0f), tpf);
    
    //mSunDir = glm::vec3(glm::sin(mIago), -1.f, glm::cos(mIago));
    //mSunDir = glm::normalize(mSunDir);
    
    mSunDir = glm::vec3(-1.f, -1.f, -1.f);
    mSunDir = glm::normalize(mSunDir);
    
    mSunViewMatr = glm::lookAt(-mSunDir, glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
    mSunProjMatr = glm::ortho(-10.f, 10.f, -10.f, 10.f, -10.f, 10.f);
    
    // Sun? buffer
    glViewport(0, 0, mSunTextureWidth, mSunTextureWidth);
    glBindFramebuffer(GL_FRAMEBUFFER, mSunFrameBuffer);
    glDepthMask(GL_TRUE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glDisable(GL_BLEND);
    glClear(GL_DEPTH_BUFFER_BIT);
    
    // Lazy
    rootNode->render(mSunViewMatr, mSunProjMatr);
    //mAxesModel->render(mSunViewMatr, mSunProjMatr, testMM);
    
    // G-buffer render
    glViewport(0, 0, 1280, 720);
    glBindFramebuffer(GL_FRAMEBUFFER, mGFramebuffer);
    glClearColor(0.f, 0.f, 0.f, 1.f);
    glDepthMask(GL_TRUE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glDisable(GL_BLEND);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
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
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    if(keyStates[SDL_GetScancodeFromKey(SDLK_5)]) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        debugShow.x = 1.f;
    }
    
    rootNode->render(viewMat, projMat);
    //mAxesModel->render(viewMat, projMat, testMM);
    
    // Screen render
    glViewport(0, 0, 1280, 720);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDepthMask(GL_TRUE);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glDisable(GL_BLEND);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    
    
    if(debugShow != glm::vec4(0.f)) {
        glUseProgram(mDebugScreenShader.shaderProg->getHandle());
        
        glm::mat4 invViewProjMat = glm::inverse(projMat * viewMat);
        glUniformMatrix4fv(mDebugScreenShader.shaderProg->getInvViewProjMatrixUnif(), 1, GL_FALSE, glm::value_ptr(invViewProjMat));
        glUniform4fv(mDebugScreenShader.viewHandle, 1, glm::value_ptr(debugShow));
        
        glActiveTexture(GL_TEXTURE0 + 0);
        glBindTexture(GL_TEXTURE_2D, mGDiffuseTexture);
        glUniform1i(mDebugScreenShader.diffuseHandle, 0);
        
        glActiveTexture(GL_TEXTURE0 + 1);
        glBindTexture(GL_TEXTURE_2D, mGNormalTexture);
        glUniform1i(mDebugScreenShader.normalHandle, 1);
        
        glActiveTexture(GL_TEXTURE0 + 2);
        glBindTexture(GL_TEXTURE_2D, mGDepthStencilTexture);
        glUniform1i(mDebugScreenShader.depthHandle, 2);
        
        glBindVertexArray(mFullscreenVao);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        
        glBindVertexArray(0);
        
        glUseProgram(0);
    }
    else {
        glUseProgram(mScreenShader.shaderProg->getHandle());
        
        glm::mat4 invViewProjMat = glm::inverse(projMat * viewMat);
        glUniformMatrix4fv(mScreenShader.shaderProg->getInvViewProjMatrixUnif(), 1, GL_FALSE, glm::value_ptr(invViewProjMat));
        glm::mat4 sunViewProjMat = mSunProjMatr * mSunViewMatr;
        glUniformMatrix4fv(mScreenShader.shaderProg->getSunViewProjMatrixUnif(), 1, GL_FALSE, glm::value_ptr(sunViewProjMat));
        glUniform3fv(mScreenShader.sunDirectionHandle, 1, glm::value_ptr(mSunDir));
        
        glActiveTexture(GL_TEXTURE0 + 0);
        glBindTexture(GL_TEXTURE_2D, mGDiffuseTexture);
        glUniform1i(mScreenShader.diffuseHandle, 0);
        
        glActiveTexture(GL_TEXTURE0 + 1);
        glBindTexture(GL_TEXTURE_2D, mGNormalTexture);
        glUniform1i(mScreenShader.normalHandle, 1);
        
        glActiveTexture(GL_TEXTURE0 + 2);
        glBindTexture(GL_TEXTURE_2D, mGDepthStencilTexture);
        glUniform1i(mScreenShader.depthHandle, 2);
        
        glActiveTexture(GL_TEXTURE0 + 3);
        glBindTexture(GL_TEXTURE_2D, mSunDepthTexture);
        glUniform1i(mScreenShader.sunDepthHandle, 3);
        
        glBindVertexArray(mFullscreenVao);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        
        glBindVertexArray(0);
        
        glUseProgram(0);
    }
    
    
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
    
    fpsCounter->render(viewMatOverlay, projMatOverlay, testMM);
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
}
}

