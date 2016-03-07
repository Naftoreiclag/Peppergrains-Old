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

#include <iostream>
#include <sstream>

#include "glm/gtx/string_cast.hpp"

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
    const std::vector<ShaderProgramResource::Sampler2DControl>& sampler2DControls = mDLightShaderProg->getSampler2Ds();
    for(std::vector<ShaderProgramResource::Sampler2DControl>::const_iterator iter = sampler2DControls.begin(); iter != sampler2DControls.end(); ++ iter) {
        const ShaderProgramResource::Sampler2DControl& entry = *iter;
        
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

    glEnableVertexAttribArray(mGBufferShaderProg->getPosAttrib());
    glVertexAttribPointer(mGBufferShaderProg->getPosAttrib(), 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (void*) (0 * sizeof(GLfloat)));

    glBindVertexArray(0);
}

// Lifecycle
void SandboxGameLayer::onBegin() {

    glm::mat4 projMatOverlay = glm::ortho(0.f, (float) 1.f, 0.f, (float) 1.f);
    
    glm::vec4 test(0.0f, 0.0f, 0.0f, 1.0f);
    test = projMatOverlay * test;
    
    std::cout << glm::to_string(test) << std::endl;

    ResourceManager* resman = ResourceManager::getSingleton();
    
    // Locate where to send the sampler
    {
        mGBufferShaderProg = resman->findShaderProgram("GBuffer.shaderProgram");
        mGBufferShaderProg->grab();
        const std::vector<ShaderProgramResource::Sampler2DControl>& sampler2DControls = mGBufferShaderProg->getSampler2Ds();
        for(std::vector<ShaderProgramResource::Sampler2DControl>::const_iterator iter = sampler2DControls.begin(); iter != sampler2DControls.end(); ++ iter) {
            const ShaderProgramResource::Sampler2DControl& entry = *iter;
            
            if(entry.name == "diffuse") {
                mDiffuseHandle = entry.handle;
            }
            else if(entry.name == "normal") {
                mNormalHandle = entry.handle;
            }
            else if(entry.name == "position") {
                mPositionHandle = entry.handle;
            }
            /*
            else if(entry.name == "bright") {
                mBrightHandle = entry.handle;
            }
            */
        }
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

        glEnableVertexAttribArray(mGBufferShaderProg->getPosAttrib());
        glVertexAttribPointer(mGBufferShaderProg->getPosAttrib(), 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*) (0 * sizeof(GLfloat)));
        glEnableVertexAttribArray(mGBufferShaderProg->getUVAttrib());
        glVertexAttribPointer(mGBufferShaderProg->getUVAttrib(), 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*) (2 * sizeof(GLfloat)));

        glBindVertexArray(0);
    }
    
    // Create renderbuffer/textures for deferred shading
    {
        // Diffuse mapping
        glGenTextures(1, &mDiffuseTexture);
        glBindTexture(GL_TEXTURE_2D, mDiffuseTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1280, 720, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glBindTexture(GL_TEXTURE_2D, 0);
        
        // Normal mapping
        glGenTextures(1, &mNormalTexture);
        glBindTexture(GL_TEXTURE_2D, mNormalTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 1280, 720, 0, GL_RGB, GL_FLOAT, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glBindTexture(GL_TEXTURE_2D, 0);
        
        // Position mapping
        glGenTextures(1, &mPositionTexture);
        glBindTexture(GL_TEXTURE_2D, mPositionTexture);
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
        glGenTextures(1, &mDepthStencilTexture);
        glBindTexture(GL_TEXTURE_2D, mDepthStencilTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, 1280, 720, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glBindTexture(GL_TEXTURE_2D, 0);
        
        /*
        glGenRenderbuffers(1, &mDepthStencilRenderBuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, mDepthStencilRenderBuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 1280, 720);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        */
    }
    
    // Create framebuffer
    {
        glGenFramebuffers(1, &mFramebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, mFramebuffer);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mDiffuseTexture, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, mNormalTexture, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, mPositionTexture, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, mDepthStencilTexture, 0);
        //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, mBrightTexture, 0);
        //glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mDepthStencilRenderBuffer);
        
        GLuint colorAttachments[] = {
            GL_COLOR_ATTACHMENT0,
            GL_COLOR_ATTACHMENT1,
            GL_COLOR_ATTACHMENT2
            //GL_COLOR_ATTACHMENT3
        };
        glDrawBuffers(3, colorAttachments);
        
        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE) {
            std::cout << "Complete" << std::endl;
        }
        else {
            std::cout << "Incomplete" << std::endl;
        }
        
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        
    }
    
    makeLightVao();

    rootNode = new SceneNode();
    friendNodeX = new SceneNode();
    friendNodeY = new SceneNode();
    friendNodeZ = new SceneNode();
    testPlaneNode = new SceneNode();
    
    testPlaneNode->grabModel(resman->findModel("Iago.model"));
    rootNode->addChild(testPlaneNode);
    
    rainstormFont = resman->findFont("Rainstorm.font");
    rainstormFont->grab();

    fpsCounter = new TextModel(rainstormFont, "FPS: Calculating...");
    fpsCounter->grab();

    friendNodeX->grabModel(resman->findModel("JellySmoothTorus.model"));
    friendNodeY->grabModel(resman->findModel("JellySmoothTorus.model"));
    friendNodeZ->grabModel(resman->findModel("JellySmoothTorus.model"));

    rootNode->addChild(friendNodeX);
    rootNode->addChild(friendNodeY);
    rootNode->addChild(friendNodeZ);
    
    friendNodeX->move(glm::vec3(-2.f, 2.f, 0.f));
    friendNodeY->move(glm::vec3(0.f, 2.f, 0.f));
    friendNodeZ->move(glm::vec3(2.f, 2.f, 0.f));

    mAxesModel = new AxesModel();
    mAxesModel->grab();

    fps = 0.f;
    fpsWeight = 0.85f;

    oneSecondTimer = 0.f;
}
void SandboxGameLayer::onEnd() {
    
    glDeleteBuffers(1, &mFullscreenIbo);
    glDeleteBuffers(1, &mFullscreenVbo);
    
    glDeleteTextures(1, &mDiffuseTexture);
    glDeleteTextures(1, &mNormalTexture);
    glDeleteTextures(1, &mPositionTexture);
    glDeleteTextures(1, &mDepthStencilTexture);
    //glDeleteTextures(1, &mBrightTexture);

    mGBufferShaderProg->drop();

    glDeleteVertexArrays(1, &mFullscreenVao);
    
    fpsCounter->drop();
    
    mAxesModel->drop();

    friendNodeX->dropModel();
    friendNodeY->dropModel();
    friendNodeZ->dropModel();
    
    testPlaneNode->dropModel();
    
    rainstormFont->drop();
    
    glDeleteFramebuffers(1, &mFramebuffer);
}

// Ticks
void SandboxGameLayer::onTick(float tpf, const Uint8* keyStates) {
    
    glm::mat4 viewMat = glm::lookAt(glm::vec3(2.f, 4.f, 3.f), glm::vec3(0.f, 2.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
    glm::mat4 projMat = glm::perspective(glm::radians(90.f), 1280.f / 720.f, 1.f, 10.f);

    glm::mat4 viewMatOverlay;
    glm::mat4 projMatOverlay = glm::ortho(0.f, (float) 1280, 0.f, (float) 720);
    glm::mat4 testMM;
    

    
    friendNodeX->rotate(glm::vec3(1.0f, 0.0f, 0.0f), (float) tpf);
    friendNodeY->rotate(glm::vec3(0.0f, 1.0f, 0.0f), (float) tpf);
    friendNodeZ->rotate(glm::vec3(0.0f, 0.0f, 1.0f), (float) tpf);
    
    glBindFramebuffer(GL_FRAMEBUFFER, mFramebuffer);
    glClearColor(0.f, 0.f, 0.f, 1.f);
    glDepthMask(GL_TRUE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glDisable(GL_BLEND);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    rootNode->render(viewMat, projMat);
    mAxesModel->render(viewMat, projMat, testMM);
    
    /*
    glDepthMask(GL_FALSE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_ONE, GL_ONE);
    
    // Render lights here
    glUseProgram(mDLightShaderProg->getHandle());
    
    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_2D, mDiffuseTexture);
    glUniform1i(mDLightDiff, 0);
    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_2D, mNormalTexture);
    glUniform1i(mDLightNorm, 1);
    
    glBindVertexArray(mDLightVao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    
    glBindVertexArray(0);
    
    glUseProgram(0);
    */
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDepthMask(GL_TRUE);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glDisable(GL_BLEND);
    
    glUseProgram(mGBufferShaderProg->getHandle());
    
    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_2D, mDiffuseTexture);
    glUniform1i(mDiffuseHandle, 0);
    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_2D, mNormalTexture);
    glUniform1i(mNormalHandle, 1);
    glActiveTexture(GL_TEXTURE0 + 2);
    glBindTexture(GL_TEXTURE_2D, mPositionTexture);
    glUniform1i(mPositionHandle, 2);
    /*
    glBindTexture(GL_TEXTURE_2D, mBrightTexture);
    glUniform1i(mBrightHandle, 3);
    */
    
    glBindVertexArray(mFullscreenVao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    
    glBindVertexArray(0);
    
    glUseProgram(0);
    
    
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

bool SandboxGameLayer::onWindowSizeUpdate(const SDL_WindowEvent& event) {
    std::cout << event.data1 << ", " << event.data2 << std::endl;
}
}

