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

// Lifecycle
void SandboxGameLayer::onBegin() {

    glm::mat4 projMatOverlay = glm::ortho(0.f, (float) 1.f, 0.f, (float) 1.f);
    
    glm::vec4 test(0.0f, 0.0f, 0.0f, 1.0f);
    test = projMatOverlay * test;
    
    std::cout << glm::to_string(test) << std::endl;

    ResourceManager* resman = ResourceManager::getSingleton();
    
    mShaderProg = resman->findShaderProgram("ScreenTexture.shaderProgram");
    mShaderProg->grab();
    
    mTestTexture = resman->findTexture("128Rose.texture");
    mTestTexture->grab();
    
    
    std::cout << mShaderProg->getHandle() << std::endl;
    std::cout << mShaderProg->needsPosAttrib() << std::endl;
    std::cout << mShaderProg->needsUVAttrib() << std::endl;
    
    // Locate where to send the sampler
    {
        const std::vector<ShaderProgramResource::Sampler2DControl>& sampler2DControls = mShaderProg->getSampler2Ds();
        for(std::vector<ShaderProgramResource::Sampler2DControl>::const_iterator iter = sampler2DControls.begin(); iter != sampler2DControls.end(); ++ iter) {
            const ShaderProgramResource::Sampler2DControl& entry = *iter;
            mTextureHandle = entry.handle;
            std::cout << "asdf " << mTextureHandle << std::endl;
            break; // (Might want other samplers in the future)
        }
    }
    
    // Fullscreen quad
    {
        GLfloat vertices[] = {
            -1.f,  1.f, 0.f, 1.f,
             1.f,  1.f, 1.f, 1.f,
            -1.f, -1.f, 0.f, 0.f,
             1.f, -1.f, 1.f, 0.f
        };
        GLuint indices[] = {
            0, 1, 2,
            1, 3, 2,
        };
        
        glGenBuffers(1, &mVertexBufferObject);
        glBindBuffer(GL_ARRAY_BUFFER, mVertexBufferObject);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glGenBuffers(1, &mIndexBufferObject);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBufferObject);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        
        glGenVertexArrays(1, &mVertexArrayObject);
        glBindVertexArray(mVertexArrayObject);

        glBindBuffer(GL_ARRAY_BUFFER, mVertexBufferObject);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBufferObject);

        glEnableVertexAttribArray(mShaderProg->getPosAttrib());
        glVertexAttribPointer(mShaderProg->getPosAttrib(), 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*) (0 * sizeof(GLfloat)));
        glEnableVertexAttribArray(mShaderProg->getUVAttrib());
        glVertexAttribPointer(mShaderProg->getUVAttrib(), 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*) (2 * sizeof(GLfloat)));

        glBindVertexArray(0);
    }
    
    // Create renderbuffer/textures for deferred shading
    {
        glGenTextures(1, &mColorTexture);
        glBindTexture(GL_TEXTURE_2D, mColorTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1280, 720, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glBindTexture(GL_TEXTURE_2D, 0);
        
        glGenTextures(1, &mNormalTexture);
        glBindTexture(GL_TEXTURE_2D, mNormalTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 1280, 720, 0, GL_RGB, GL_FLOAT, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glBindTexture(GL_TEXTURE_2D, 0);
        
        glGenTextures(1, &mPositionTexture);
        glBindTexture(GL_TEXTURE_2D, mPositionTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 1280, 720, 0, GL_RGB, GL_FLOAT, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glBindTexture(GL_TEXTURE_2D, 0);
        
        glGenRenderbuffers(1, &mDepthStencilRenderBuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, mDepthStencilRenderBuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 1280, 720);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
    }
    
    // Create framebuffer
    {
        glGenFramebuffers(1, &mFramebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, mFramebuffer);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mColorTexture, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, mNormalTexture, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, mPositionTexture, 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mDepthStencilRenderBuffer);
        
        GLuint colorAttachments[] = {
            GL_COLOR_ATTACHMENT0,
            GL_COLOR_ATTACHMENT1,
            GL_COLOR_ATTACHMENT2
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

    rootNode = new SceneNode();
    friendNode1 = new SceneNode();
    friendNode2 = new SceneNode();
    friendNode3 = new SceneNode();
    
    rainstormFont = resman->findFont("Rainstorm.font");
    rainstormFont->grab();

    fpsCounter = new TextModel(rainstormFont, "FPS: Calculating...");
    fpsCounter->grab();

    friendNode1->grabModel(resman->findModel("JellyUFO.model"));
    friendNode2->grabModel(resman->findModel("JellyUFO.model"));
    friendNode3->grabModel(resman->findModel("JellyUFO.model"));

    rootNode->addChild(friendNode1);
    rootNode->addChild(friendNode2);
    rootNode->addChild(friendNode3);
    
    friendNode1->move(glm::vec3(-2.f, 0.f, 0.f));
    friendNode3->move(glm::vec3(2.f, 0.f, 0.f));

    fps = 0.f;
    fpsWeight = 0.85f;

    oneSecondTimer = 0.f;
}
void SandboxGameLayer::onEnd() {
    
    glDeleteBuffers(1, &mIndexBufferObject);
    glDeleteBuffers(1, &mVertexBufferObject);
    
    glDeleteTextures(1, &mColorTexture);
    glDeleteTextures(1, &mNormalTexture);
    glDeleteTextures(1, &mPositionTexture);

    mShaderProg->drop();

    glDeleteVertexArrays(1, &mVertexArrayObject);
    
    fpsCounter->drop();
    mTestTexture->drop();


    friendNode1->dropModel();
    friendNode2->dropModel();
    friendNode3->dropModel();
    
    rainstormFont->drop();
    
    glDeleteFramebuffers(1, &mFramebuffer);
}

// Ticks
void SandboxGameLayer::onTick(float tpf, const Uint8* keyStates) {
    
    glm::mat4 viewMat = glm::lookAt(glm::vec3(0.f, 0.f, -3.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
    glm::mat4 projMat = glm::perspective(glm::radians(90.f), 1280.f / 720.f, 1.f, 10.f);

    glm::mat4 viewMatOverlay;
    glm::mat4 projMatOverlay = glm::ortho(0.f, (float) 1280, 0.f, (float) 720);
    glm::mat4 testMM;

    friendNode1->rotate(glm::vec3(1.0f, 0.0f, 0.0f), (float) tpf);
    friendNode2->rotate(glm::vec3(0.0f, 1.0f, 0.0f), (float) tpf);
    friendNode3->rotate(glm::vec3(0.0f, 0.0f, 1.0f), (float) tpf);
    
    glBindFramebuffer(GL_FRAMEBUFFER, mFramebuffer);
    glClearColor(0.0f, 0.0f, 0.0f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    
    rootNode->render(viewMat, projMat);
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    
    glUseProgram(mShaderProg->getHandle());
    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_2D, mPositionTexture);
    glUniform1i(mTextureHandle, 0);
    glBindVertexArray(mVertexArrayObject);
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

