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

#include "DeferredRenderer.hpp"

#include "ResourceManager.hpp"

namespace pgg {

void DeferredRenderer::load() {
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
    
    mSky.shadowMapResolution = 1024;
    mSky.direction = glm::normalize(glm::vec3(-1.f, -1.f, -1.f));
    mSky.location = glm::vec3(1.f, 1.f, 1.f);
    mSky.sunModel = new SunLightModel(glm::vec3(1.0f, 1.0f, 1.0f));
    mSky.sunModel->grab();
    
    // Depth mapping
    glGenTextures(PGG_NUM_SUN_CASCADES, mSky.depthTextures);
    
    for(uint8_t i = 0; i < PGG_NUM_SUN_CASCADES; ++ i) {
        glBindTexture(GL_TEXTURE_2D, mSky.depthTextures[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, mSky.shadowMapResolution, mSky.shadowMapResolution, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    
    glGenFramebuffers(PGG_NUM_SUN_CASCADES, mSky.framebuffers);
    for(uint8_t i = 0; i < PGG_NUM_SUN_CASCADES; ++ i) {
        glBindFramebuffer(GL_FRAMEBUFFER, mSky.framebuffers[i]);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, mSky.depthTextures[i], 0);
        
        // No color buffers
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    this->setCameraProjection(glm::radians(90.f), 0.2f, 400.f);
}

void DeferredRenderer::unload() {
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
    
    glDeleteTextures(4, mSky.depthTextures);
    glDeleteFramebuffers(4, mSky.framebuffers);
    mSky.sunModel->drop();
    
    delete this;
}

void DeferredRenderer::renderFrame(SceneNode* mRootNode, glm::vec4 debugShow, bool wireframe) {
    // Calculate shadow map cascades
    {
        mSky.viewMatrix = glm::lookAt(mSky.location - mSky.direction, mSky.location, glm::vec3(0.f, 1.f, 0.f));
        
        glm::mat4 sunMatr = mSky.viewMatrix;
        
        for(uint8_t i = 0; i < PGG_NUM_SUN_CASCADES; ++ i) {
            
            glm::mat4 projMatrix = glm::perspective(mCamera.fov, mCamera.aspect, mCamera.cascadeBorders[i], mCamera.cascadeBorders[i + 1]);
            glm::mat4 invVPMatrix = glm::inverse(projMatrix * mCamera.viewMat);
            
            glm::vec3 minBB;
            glm::vec3 maxBB;
            for(uint8_t j = 0; j < 8; ++ j) {
                glm::vec4 corner(
                    (j & (1 << 0)) ? -1.f : 1.f,
                    (j & (1 << 1)) ? -1.f : 1.f,
                    (j & (1 << 2)) ? -1.f : 1.f,
                    1.f
                );
                
                glm::vec4 cornerWorldSpace = invVPMatrix * corner;
                cornerWorldSpace /= cornerWorldSpace.w; // Perspective divide
                
                // cornerWorldSpace is a coordinate in world space for this corner of the view fustrum
                
                // find the location in "sunspace"
                glm::vec4 locInSun = sunMatr * cornerWorldSpace;
                
                if(j == 0) {
                    minBB = glm::vec3(locInSun);
                    maxBB = glm::vec3(locInSun);
                } else {
                    if(locInSun.x < minBB.x) { minBB.x = locInSun.x; }
                    if(locInSun.y < minBB.y) { minBB.y = locInSun.y; }
                    if(locInSun.z < minBB.z) { minBB.z = locInSun.z; }
                    if(locInSun.x > maxBB.x) { maxBB.x = locInSun.x; }
                    if(locInSun.y > maxBB.y) { maxBB.y = locInSun.y; }
                    if(locInSun.z > maxBB.z) { maxBB.z = locInSun.z; }
                }
            }
            
            mSky.projectionMatrices[i] = glm::ortho(minBB.x, maxBB.x, minBB.y, maxBB.y, -50.f, 50.f);
        }
    }
    
    // Sunlight shadow pass
    {
        for(uint8_t i = 0; i < PGG_NUM_SUN_CASCADES; ++ i) {
            glViewport(0, 0, mSky.shadowMapResolution, mSky.shadowMapResolution);
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mSky.framebuffers[i]);
            glDepthMask(GL_TRUE);
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LESS);
            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);
            glDisable(GL_BLEND);
            glClear(GL_DEPTH_BUFFER_BIT);
            
            Model::RenderPass sunRPC(Model::RenderPassType::SHADOW);
            sunRPC.viewMat = mSky.viewMatrix;
            sunRPC.projMat = mSky.projectionMatrices[i];
            sunRPC.camPos = mSky.direction * 100000.f;
            mRootNode->render(sunRPC);
        }
    }
    
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
        
        Model::RenderPass rootNodeRPC(Model::RenderPassType::GEOMETRY);
        rootNodeRPC.viewMat = mCamera.viewMat;
        rootNodeRPC.projMat = mCamera.projMat;
        rootNodeRPC.camPos = mCamera.position;
        rootNodeRPC.calculateFustrumAABB();
        mRootNode->render(rootNodeRPC);
    }
    
    // Brightness Render
    {
        // Clear brightness
        {
            GLuint colorAttachment[] = {
                GL_COLOR_ATTACHMENT2
            };
            glDrawBuffers(1, colorAttachment);
            // Ambient light
            glClearColor(0.2f, 0.2f, 0.2f, 1.f);
            glClear(GL_COLOR_BUFFER_BIT);
        }
        
        // Do not write to the depth buffer
        glDepthMask(GL_FALSE);
        
        // Filled polygons
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        
        // glm::mat4 sunViewProjMat = mSky.sunBasicProjectionMatrix * mSky.sunBasicViewMatrix;
        
        // Render pass config
        Model::RenderPass brightRPC(Model::RenderPassType::LOCAL_LIGHTS);
        brightRPC.viewMat = mCamera.viewMat;
        brightRPC.projMat = mCamera.projMat;
        brightRPC.camPos = mCamera.position;
        // TODO: something else
        for(uint8_t i = 0; i < PGG_NUM_SUN_CASCADES + 1; ++ i) {
            brightRPC.cascadeBorders[i] = mCamera.cascadeBorders[i];
        }
        brightRPC.depthStencilTexture = mGBuff.depthStencilTexture;
        brightRPC.normalTexture = mGBuff.normalTexture;
        for(uint8_t i = 0; i < PGG_NUM_SUN_CASCADES; ++ i) {
            brightRPC.sunViewProjMatr[i] = mSky.projectionMatrices[i] * mSky.viewMatrix;
            brightRPC.sunDepthTexture[i] = mSky.depthTextures[i];
        }
        
        // Render local lights
        mRootNode->render(brightRPC);
        
        // Render global lights
        {
            // Prepare stencil
            // This also fills the diffuse buffer with white where the sky is
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
                mRootNode->render(brightRPC);
                mSky.sunModel->render(brightRPC, glm::inverse(mSky.viewMatrix));
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
    
DeferredRenderer::DeferredRenderer(uint32_t width, uint32_t height)
: mScreenWidth(width)
, mScreenHeight(height) {
}

DeferredRenderer::~DeferredRenderer() {
}

void DeferredRenderer::setSunDirection(const glm::vec3& dirSunAiming) {
    mSky.direction = dirSunAiming;
}
void DeferredRenderer::setCameraViewMatrix(const glm::mat4& camViewMatrix) {
    mCamera.viewMat = camViewMatrix;
    mCamera.position = glm::vec3(-mCamera.viewMat[3][0], -mCamera.viewMat[3][1], -mCamera.viewMat[3][2]);
}

const glm::vec3& DeferredRenderer::getCameraLocation() const {
    return mCamera.position;
}

void DeferredRenderer::setCameraProjection(float fov, float nearDepth, float farDepth) {
    mCamera.fov = fov;
    mCamera.aspect = ((float) mScreenWidth) / ((float) mScreenHeight);
    mCamera.nearDepth = nearDepth;
    mCamera.farDepth = farDepth;
    mCamera.cascadeBorders[0] = mCamera.nearDepth;
    mCamera.cascadeBorders[PGG_NUM_SUN_CASCADES] = mCamera.farDepth;
    for(uint8_t i = 1; i < PGG_NUM_SUN_CASCADES; ++ i) {
        // TODO: replace + 1.f with something more meaningful
        float near = mCamera.nearDepth + 1.f;
        float far = mCamera.farDepth;
        mCamera.cascadeBorders[i] = near * std::pow(far / near, ((float) i) / ((float) PGG_NUM_SUN_CASCADES));
    }
    mCamera.projMat = glm::perspective(mCamera.fov, mCamera.aspect, mCamera.nearDepth, mCamera.farDepth);
}
const glm::mat4& DeferredRenderer::getCameraProjectionMatrix() const {
    return mCamera.projMat;
}
const glm::mat4& DeferredRenderer::getCameraViewMatrix() const {
    return mCamera.viewMat;
}

}
