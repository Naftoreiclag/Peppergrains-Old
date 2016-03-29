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

#include "OverworldGameLayer.hpp"

#include <cassert>
#include <iostream>
#include <sstream>

#include "glm/gtx/string_cast.hpp"
#include "SDL2/SDL.h"

#include "SunLightModel.hpp"
#include "DirectionalLightModel.hpp"
#include "PointLightModel.hpp"
#include "GrassModel.hpp"

namespace pgg
{

OverworldGameLayer::OverworldGameLayer(uint32_t width, uint32_t height)
: mScreenWidth(width)
, mScreenHeight(height) {
}

OverworldGameLayer::~OverworldGameLayer() {
}

// Lifecycle
void OverworldGameLayer::onBegin() {
    SDL_SetRelativeMouseMode(SDL_TRUE);
    
    loadGBuffer();
    loadSun();

    ResourceManager* resman = ResourceManager::getSingleton();

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
    iago->grabModel(new PointLightModel(glm::vec3(0.0f, 1.0f, 1.0f), 1.00f));
    iago->setLocalTranslation(glm::vec3(0.f, 1.5f, 3.f));
    rootNode->addChild(iago);

    fps = 0.f;
    mIago = 0.f;
    fpsWeight = 0.85f;
    
    mDebugWireframe = false;

    oneSecondTimer = 0.f;
}
void OverworldGameLayer::onEnd() {
    unloadGBuffer();
    unloadSun();
    
    fpsCounter->drop();
    mAxesModel->drop();
    friendNodeX->dropModel();
    friendNodeY->dropModel();
    friendNodeZ->dropModel();
    testPlaneNode->dropModel();
    rainstormFont->drop();
}

void OverworldGameLayer::onTick(float tpf, const Uint8* keyStates) {
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

    
    mCamera.viewMat = glm::inverse(mCamRollNode->calcWorldTransform());
    mCamera.projMat = glm::perspective(glm::radians(90.f), ((float) mScreenWidth) / ((float) mScreenHeight), 0.1f, 500.f);
    
    mIago += tpf;
    
    //iago->setLocalTranslation(glm::vec3(0.f, 1.5f + (glm::sin(mIago) * 1.5f), 3.f));
    iago->setLocalScale(glm::vec3(1.0f + (glm::sin(mIago) * 0.5f)));
    
    friendNodeY->rotate(glm::vec3(0.0f, 1.0f, 0.0f), tpf);
    friendNodeZ->rotate(glm::vec3(0.0f, 0.0f, 1.0f), tpf);

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
    
    if(keyStates[SDL_GetScancodeFromKey(SDLK_q)]) {
        mSky.sunDirection = glm::vec3(mCamRollNode->calcWorldTransform() * glm::vec4(0.f, 0.f, -1.f, 0.f));
    }
    mSky.sunViewMatr = glm::lookAt(mSky.sunPosition - mSky.sunDirection, mSky.sunPosition, glm::vec3(0.f, 1.f, 0.f));
    mSky.sunProjMatr = glm::ortho(-10.f, 10.f, -10.f, 10.f, -10.f, 10.f);
    
    renderFrame(debugShow, mDebugWireframe);
    
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
    
    glm::mat4 viewMatOverlay;
    glm::mat4 projMatOverlay = glm::ortho(0.f, (float) mScreenWidth, 0.f, (float) mScreenHeight);
    
    Model::RenderPassConfiguration fpsRPC(Model::RenderPassType::SCREEN);
    fpsRPC.viewMat = viewMatOverlay;
    fpsRPC.projMat = projMatOverlay;
    fpsCounter->render(fpsRPC, glm::mat4());
}

void OverworldGameLayer::loadGBuffer() {
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
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, mGBuff.brightTexture, 0);
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
    
    ResourceManager* resman = ResourceManager::getSingleton();
    // GBuffer shader
    {
        mScreenShader.shaderProg = resman->findShaderProgram("GBuffer.shaderProgram");
        mScreenShader.shaderProg->grab();
        const std::vector<ShaderProgramResource::Control>& sampler2DControls = mScreenShader.shaderProg->getUniformSampler2Ds();
        for(std::vector<ShaderProgramResource::Control>::const_iterator iter = sampler2DControls.begin(); iter != sampler2DControls.end(); ++ iter) {
            const ShaderProgramResource::Control& entry = *iter;
            
            if(entry.name == "diffuse") {
                mScreenShader.diffuseHandle = entry.handle;
            }
            else if(entry.name == "bright") {
                mScreenShader.brightHandle = entry.handle;
            }
        }
    }
    // Debug shader
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
    // Sky stencil shader
    {
        mSkyStencilShader.shaderProg = resman->findShaderProgram("SkyStencil.shaderProgram");
        mSkyStencilShader.shaderProg->grab();
    }
    // Fill screen shader
    {
        mFillScreenShader.shaderProg = resman->findShaderProgram("FillScreen.shaderProgram");
        mFillScreenShader.shaderProg->grab();
        const std::vector<ShaderProgramResource::Control>& vec3Controls = mFillScreenShader.shaderProg->getUniformVec3s();
        for(std::vector<ShaderProgramResource::Control>::const_iterator iter = vec3Controls.begin(); iter != vec3Controls.end(); ++ iter) {
            const ShaderProgramResource::Control& entry = *iter;
            
            if(entry.name == "color") {
                mFillScreenShader.colorHandle = entry.handle;
            }
        }
    }
    // Fullscreen quad
    {
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
        glVertexAttribPointer(mScreenShader.shaderProg->getPosAttrib(), 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (void*) (0 * sizeof(GLfloat)));
        
        glEnableVertexAttribArray(mDebugScreenShader.shaderProg->getPosAttrib());
        glVertexAttribPointer(mDebugScreenShader.shaderProg->getPosAttrib(), 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (void*) (0 * sizeof(GLfloat)));
        
        glEnableVertexAttribArray(mSkyStencilShader.shaderProg->getPosAttrib());
        glVertexAttribPointer(mSkyStencilShader.shaderProg->getPosAttrib(), 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (void*) (0 * sizeof(GLfloat)));

        glBindVertexArray(0);
    }
}
void OverworldGameLayer::unloadGBuffer() {
    glDeleteBuffers(1, &mFullscreenIbo);
    glDeleteBuffers(1, &mFullscreenVbo);
    glDeleteVertexArrays(1, &mFullscreenVao);
    
    glDeleteTextures(1, &mGBuff.diffuseTexture);
    glDeleteTextures(1, &mGBuff.normalTexture);
    glDeleteTextures(1, &mGBuff.depthStencilTexture);
    glDeleteTextures(1, &mGBuff.brightTexture);
    glDeleteFramebuffers(1, &mGBuff.gFramebuffer);

    mScreenShader.shaderProg->drop();
    mDebugScreenShader.shaderProg->drop();
    mSkyStencilShader.shaderProg->drop();
    mFillScreenShader.shaderProg->drop();
}

void OverworldGameLayer::loadSun() {
    mSky.sunTextureSize = 1024;
    mSky.sunDirection = glm::normalize(glm::vec3(-1.f, -1.f, -1.f));
    mSky.sunModel = new SunLightModel(glm::vec3(1.0f, 1.0f, 1.0f));
    mSky.sunModel->grab();
    
    // DepthStencil mapping
    glGenTextures(1, &mSky.sunDepthTexture);
    glBindTexture(GL_TEXTURE_2D, mSky.sunDepthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, mSky.sunTextureSize, mSky.sunTextureSize, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    glGenFramebuffers(1, &mSky.sunFramebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, mSky.sunFramebuffer);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, mSky.sunDepthTexture, 0);
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
void OverworldGameLayer::unloadSun() {
    glDeleteTextures(1, &mSky.sunDepthTexture);
    glDeleteFramebuffers(1, &mSky.sunFramebuffer);
    mSky.sunModel->drop();
}

void OverworldGameLayer::renderFrame(glm::vec4 debugShow, bool wireframe) {
    // Sun? buffer
    glViewport(0, 0, mSky.sunTextureSize, mSky.sunTextureSize);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mSky.sunFramebuffer);
    glDepthMask(GL_TRUE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glDisable(GL_BLEND);
    glClear(GL_DEPTH_BUFFER_BIT);
    
    Model::RenderPassConfiguration sunRPC(Model::RenderPassType::SHADOW);
    sunRPC.viewMat = mSky.sunViewMatr;
    sunRPC.projMat = mSky.sunProjMatr;
    rootNode->render(sunRPC);
    
    // Geometry pass
    {
        glViewport(0, 0, mScreenWidth, mScreenHeight);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mGBuff.gFramebuffer);
        GLuint colorAttachments[] = {
            GL_COLOR_ATTACHMENT0,
            GL_COLOR_ATTACHMENT1
        };
        glDrawBuffers(2, colorAttachments);
        glClearColor(0.f, 0.f, 0.f, 1.f);
        glDepthMask(GL_TRUE);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glDisable(GL_STENCIL_TEST);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glDisable(GL_BLEND);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        if(wireframe) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
        
        Model::RenderPassConfiguration rootNodeRPC(Model::RenderPassType::GEOMETRY);
        rootNodeRPC.viewMat = mCamera.viewMat;
        rootNodeRPC.projMat = mCamera.projMat;
        rootNode->render(rootNodeRPC);
    }
    
    // Brightness Render
    {
        // Clear brightness
        {
            GLuint colorAttachment[] = {
                GL_COLOR_ATTACHMENT2
            };
            glDrawBuffers(1, colorAttachment);
            glClearColor(0.f, 0.0f, 0.f, 1.f);
            glClear(GL_COLOR_BUFFER_BIT);
        }
        
        // Do not write to the depth buffer
        glDepthMask(GL_FALSE);
        
        // Filled polygons
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        
        glm::mat4 sunViewProjMat = mSky.sunProjMatr * mSky.sunViewMatr;
        
        // Render pass config
        Model::RenderPassConfiguration brightRPC(Model::RenderPassType::LOCAL_LIGHTS);
        brightRPC.viewMat = mCamera.viewMat;
        brightRPC.projMat = mCamera.projMat;
        brightRPC.depthStencilTexture = mGBuff.depthStencilTexture;
        brightRPC.normalTexture = mGBuff.normalTexture;
        brightRPC.sunViewProjMatr = sunViewProjMat;
        brightRPC.sunDepthTexture = mSky.sunDepthTexture;
        
        // Render local lights
        rootNode->render(brightRPC);
        
        // Render global lights
        {
            // Prepare stencil
            // This also fills the diffuse buffer with white
            {
                glDrawBuffer(GL_COLOR_ATTACHMENT0);
                glEnable(GL_DEPTH_TEST);
                glDepthFunc(GL_LESS);
                glEnable(GL_STENCIL_TEST);
                glDisable(GL_CULL_FACE);
                glClearStencil(1);
                glClear(GL_STENCIL_BUFFER_BIT);
                
                glStencilFunc(GL_ALWAYS, 0, 0);
                
                // 1 = sky
                // 0 = ground
                glStencilOpSeparate(GL_FRONT_AND_BACK, GL_KEEP, GL_ZERO, GL_KEEP);
                
                glUseProgram(mSkyStencilShader.shaderProg->getHandle());
                
                glBindVertexArray(mFullscreenVao);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                glBindVertexArray(0);
                
                glUseProgram(0);
                
                // Only keep pixels that are not a part of the sky
                glStencilFunc(GL_EQUAL, 0, 0xff);
            }
            
            // Prepare blending
            {
                GLuint colorAttachments[] = {
                    GL_COLOR_ATTACHMENT2
                };
                glDrawBuffers(1, colorAttachments);
                
                glDisable(GL_DEPTH_TEST);
                glDisable(GL_CULL_FACE);
                glEnable(GL_BLEND);
                glBlendEquation(GL_FUNC_ADD);
                glBlendFunc(GL_ONE, GL_ONE);
            }
            
            // Actual rendering
            {    
                brightRPC.type = Model::RenderPassType::GLOBAL_LIGHTS;
                rootNode->render(brightRPC);
                mSky.sunModel->render(brightRPC, glm::inverse(mSky.sunViewMatr));
                
                // Ambient lighting
                glUseProgram(mFillScreenShader.shaderProg->getHandle());
                glUniform3fv(mFillScreenShader.colorHandle, 1, glm::value_ptr(glm::vec3(0.2, 0.2, 0.2)));
                glBindVertexArray(mFullscreenVao);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                glBindVertexArray(0);
                glUseProgram(0);
            }
        }
    }
    
    // Sky render
    {
        // Note: this makes use of the stencil created during the global brightness render
        
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glDisable(GL_BLEND);
        glEnable(GL_STENCIL_TEST);
        
        // Only keep pixels that are a part of the sky
        glStencilFunc(GL_EQUAL, 1, 0xff);
        
        // Populate the brightness buffer
        glDrawBuffer(GL_COLOR_ATTACHMENT2);
        
        glUseProgram(mFillScreenShader.shaderProg->getHandle());
        
        glUniform3fv(mFillScreenShader.colorHandle, 1, glm::value_ptr(glm::vec3(0.0, 2.0, 2.0)));
        
        glBindVertexArray(mFullscreenVao);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        
        glUseProgram(0);
        
    }
    
    // Screen render
    glViewport(0, 0, mScreenWidth, mScreenHeight);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glDepthMask(GL_FALSE);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_STENCIL_TEST);
    glDepthFunc(GL_EQUAL);
    glDisable(GL_CULL_FACE);
    glDisable(GL_BLEND);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    
    if(debugShow != glm::vec4(0.f)) {
        glUseProgram(mDebugScreenShader.shaderProg->getHandle());
        
        glm::mat4 invViewProjMat = glm::inverse(mCamera.projMat * mCamera.viewMat);
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
        
        glActiveTexture(GL_TEXTURE0 + 0);
        glBindTexture(GL_TEXTURE_2D, mGBuff.diffuseTexture);
        glUniform1i(mScreenShader.diffuseHandle, 0);
        
        glActiveTexture(GL_TEXTURE0 + 1);
        glBindTexture(GL_TEXTURE_2D, mGBuff.brightTexture);
        glUniform1i(mScreenShader.brightHandle, 1);
        
        glBindVertexArray(mFullscreenVao);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        
        glUseProgram(0);
    }
}

bool OverworldGameLayer::onMouseMove(const SDL_MouseMotionEvent& event) {
    float x = event.x;
    float y = event.y;
    float dx = event.xrel;
    float dy = event.yrel;
    
    mCamYawNode->rotateYaw(-dx * 0.003f);
    mCamPitchNode->rotatePitch(-dy * 0.003f);
    
    return true;
}

bool OverworldGameLayer::onWindowSizeUpdate(const SDL_WindowEvent& event) {
    std::cout << event.data1 << ", " << event.data2 << std::endl;
    
    return true;
}
}

