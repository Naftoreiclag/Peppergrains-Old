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

#include "SSAOModel.hpp"

#include "MathUtil.hpp"
#include "ResourceManager.hpp"

namespace pgg {

SSAOModel::SharedResources::SharedResources() { }
SSAOModel::SharedResources::~SharedResources() { }
SSAOModel::SharedResources* SSAOModel::SharedResources::getSharedInstance() {
    static SharedResources instance;
    return &instance;
}

void SSAOModel::SharedResources::load() {
    ResourceManager* resman = ResourceManager::getSingleton();
    mShaderProg = resman->findShaderProgram("SSAOVolume.shaderProgram");
    mShaderProg->grab();
    
    // Generate kernels
    {
        for(uint8_t i = 0; i < 64; ++ i) {
            glm::vec3 sample(
                Math::randFloat(-1.f, 1.f),
                Math::randFloat(-1.f, 1.f),
                Math::randFloat(0.f, 1.f)
            );
            sample = glm::normalize(sample) * Math::lerp(0.1f, 1.f, ((float) i) / 64.f);
            mKernels.ssao[i * 3 + 0] = sample.x;
            mKernels.ssao[i * 3 + 1] = sample.y;
            mKernels.ssao[i * 3 + 2] = sample.z;
        }
        
        for(uint8_t i = 0; i < 64; ++ i) {
            glm::vec2 noise(
                Math::randFloat(-1.f, 1.f),
                Math::randFloat(-1.f, 1.f)
            );
            noise = glm::normalize(noise);
            mKernels.normalized2DNoise[i] = noise;
        }
        
        glGenTextures(1, &mKernels.normalized2DNoiseTexture);
        glBindTexture(GL_TEXTURE_2D, mKernels.normalized2DNoiseTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 8, 8, 0, GL_RG, GL_FLOAT, mKernels.normalized2DNoise);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    
    {
        const std::vector<ShaderProgramResource::Control>& sampler2DControls = mShaderProg->getUniformSampler2Ds();
        for(std::vector<ShaderProgramResource::Control>::const_iterator iter = sampler2DControls.begin(); iter != sampler2DControls.end(); ++ iter) {
            const ShaderProgramResource::Control& entry = *iter;
            if(entry.name == "normal") {
                mNormalHandle = entry.handle;
            } else if(entry.name == "depth") {
                mDepthHandle = entry.handle;
            } else if(entry.name == "normalized2DNoise") {
                mNoiseTextureHandle = entry.handle;
            }
        }
        const std::vector<ShaderProgramResource::Control>& vec3Controls = mShaderProg->getUniformVec3s();
        for(std::vector<ShaderProgramResource::Control>::const_iterator iter = vec3Controls.begin(); iter != vec3Controls.end(); ++ iter) {
            const ShaderProgramResource::Control& entry = *iter;
            if(entry.name == "ssaoKernel") {
                mSSAOKernelHandle = entry.handle;
            } else if(entry.name == "color") {
                mColorHandle = entry.handle;
            }
        }
        const std::vector<ShaderProgramResource::Control>& vec2Controls = mShaderProg->getUniformVec2s();
        for(std::vector<ShaderProgramResource::Control>::const_iterator iter = vec2Controls.begin(); iter != vec2Controls.end(); ++ iter) {
            const ShaderProgramResource::Control& entry = *iter;
            if(entry.name == "normalized2DNoiseRatio") {
                mNoiseRatioHandle = entry.handle;
            }
        }
        const std::vector<ShaderProgramResource::Control>& floatControls = mShaderProg->getUniformFloats();
        for(std::vector<ShaderProgramResource::Control>::const_iterator iter = floatControls.begin(); iter != floatControls.end(); ++ iter) {
            const ShaderProgramResource::Control& entry = *iter;
            if(entry.name == "nearPlane") {
                mNearHandle = entry.handle;
            } else if(entry.name == "farPlane") {
                mFarHandle = entry.handle;
            }
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
    
    
    glGenBuffers(1, &mScreenVbo);
    glBindBuffer(GL_ARRAY_BUFFER, mScreenVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &mScreenIbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mScreenIbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
    glGenVertexArrays(1, &mScreenVao);
    glBindVertexArray(mScreenVao);

    glBindBuffer(GL_ARRAY_BUFFER, mScreenVbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mScreenIbo);

    glEnableVertexAttribArray(mShaderProg->getPosAttrib());
    glVertexAttribPointer(mShaderProg->getPosAttrib(), 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (void*) (0 * sizeof(GLfloat)));

    glBindVertexArray(0);
}
void SSAOModel::SharedResources::unload() {
    mShaderProg->drop();
    
    glDeleteBuffers(1, &mScreenIbo);
    glDeleteBuffers(1, &mScreenVbo);
    glDeleteVertexArrays(1, &mScreenVao);
}

void SSAOModel::SharedResources::render(const Model::RenderPass& rendPass, const glm::mat4& modelMat, const glm::vec3& lightColor) {
    if(rendPass.type != Model::RenderPass::Type::GLOBAL_LIGHTS) {
        return;
    }

    glUseProgram(mShaderProg->getHandle());
    
    mShaderProg->bindModelViewProjMatrices(glm::mat4(), rendPass.viewMat, rendPass.projMat);
    
    glUniform3fv(mSSAOKernelHandle, 64, mKernels.ssao);
    glUniform3fv(mColorHandle, 1, glm::value_ptr(lightColor));
    glUniform2fv(mNoiseRatioHandle, 1, glm::value_ptr(glm::vec2(1280.f / 8.f, 720.f / 8.f)));
    
    glUniform1fv(mNearHandle, 1, &rendPass.nearPlane);
    glUniform1fv(mFarHandle, 1, &rendPass.farPlane);
    
    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_2D, rendPass.normalTexture);
    glUniform1i(mNormalHandle, 0);
    
    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_2D, rendPass.depthStencilTexture);
    glUniform1i(mDepthHandle, 1);
    
    glActiveTexture(GL_TEXTURE0 + 2);
    glBindTexture(GL_TEXTURE_2D, mKernels.normalized2DNoiseTexture);
    glUniform1i(mNoiseTextureHandle, 2);

    glBindVertexArray(mScreenVao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    
    glUseProgram(0);
}

void SSAOModel::setColor(const glm::vec3& color) {
    mColor = color;
}

SSAOModel::SSAOModel(glm::vec3 color)
: mColor(color) {
}
SSAOModel::~SSAOModel() {
}
void SSAOModel::load() {
    mSharedRes = SharedResources::getSharedInstance();
    mSharedRes->grab();
}
void SSAOModel::unload() {
    mSharedRes->drop();
}

void SSAOModel::render(const Model::RenderPass& rendPass, const glm::mat4& modelMat) {
    mSharedRes->render(rendPass, modelMat, mColor);
}


}

