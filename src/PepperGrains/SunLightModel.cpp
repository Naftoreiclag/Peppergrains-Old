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

#include "SunLightModel.hpp"

#include "MathUtil.hpp"
#include "ResourceManager.hpp"

namespace pgg {

SunLightModel::SharedResources::SharedResources() { }
SunLightModel::SharedResources::~SharedResources() { }
SunLightModel::SharedResources* SunLightModel::SharedResources::getSharedInstance() {
    static SharedResources instance;
    return &instance;
}

void SunLightModel::SharedResources::load() {
        
    {
        for(uint8_t i = 0; i < 64; ++ i) {
            glm::vec2 sample(
                Math::randFloat(-1.f, 1.f),
                Math::randFloat(-1.f, 1.f)
            );
            sample = glm::normalize(sample) * Math::lerp(0.1f, 1.f, ((float) i) / 64.f);
            mKernels.disk[i * 2 + 0] = sample.x;
            mKernels.disk[i * 2 + 1] = sample.y;
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
    
    ResourceManager* resman = ResourceManager::getSingleton();
    mShaderProg = resman->findShaderProgram("SunLightVolume.shaderProgram");
    mMinimalShader = resman->findShaderProgram("SkyStencil.shaderProgram");
    mShaderProg->grab();
    mMinimalShader->grab();
    
    {
        const std::vector<ShaderProgramResource::Control>& sampler2DControls = mShaderProg->getUniformSampler2Ds();
        for(std::vector<ShaderProgramResource::Control>::const_iterator iter = sampler2DControls.begin(); iter != sampler2DControls.end(); ++ iter) {
            const ShaderProgramResource::Control& entry = *iter;
            if(entry.name == "normal") {
                mNormalHandle = entry.handle;
            } else if(entry.name == "depth") {
                mDepthHandle = entry.handle;
            } else if(entry.name == "sunDepth0") {
                mSunDepthHandle[0] = entry.handle;
            } else if(entry.name == "sunDepth1") {
                mSunDepthHandle[1] = entry.handle;
            } else if(entry.name == "sunDepth2") {
                mSunDepthHandle[2] = entry.handle;
            } else if(entry.name == "sunDepth3") {
                mSunDepthHandle[3] = entry.handle;
            } else if(entry.name == "normalized2DNoise") {
                mNoiseTextureHandle = entry.handle;
            }
        }
        const std::vector<ShaderProgramResource::Control>& vec3Controls = mShaderProg->getUniformVec3s();
        for(std::vector<ShaderProgramResource::Control>::const_iterator iter = vec3Controls.begin(); iter != vec3Controls.end(); ++ iter) {
            const ShaderProgramResource::Control& entry = *iter;
            if(entry.name == "direction") {
                mDirectionHandle = entry.handle;
            } else if(entry.name == "color") {
                mColorHandle = entry.handle;
            } else if(entry.name == "cameraLocation") {
                mCameraLocHandle = entry.handle;
            }
        }
        const std::vector<ShaderProgramResource::Control>& vec2Controls = mShaderProg->getUniformVec2s();
        for(std::vector<ShaderProgramResource::Control>::const_iterator iter = vec2Controls.begin(); iter != vec2Controls.end(); ++ iter) {
            const ShaderProgramResource::Control& entry = *iter;
            if(entry.name == "normalized2DNoiseRatio") {
                mNoiseScaleHandle = entry.handle;
            } else if(entry.name == "diskKernel") {
                mDiskHandle = entry.handle;
            }
        }
        const std::vector<ShaderProgramResource::Control>& floatControls = mShaderProg->getUniformFloats();
        for(std::vector<ShaderProgramResource::Control>::const_iterator iter = floatControls.begin(); iter != floatControls.end(); ++ iter) {
            const ShaderProgramResource::Control& entry = *iter;
            if(entry.name == "nearPlane") {
                mNearPlaneHandle = entry.handle;
            }
        }
        const std::vector<ShaderProgramResource::Control>& vec4Controls = mShaderProg->getUniformVec4s();
        for(std::vector<ShaderProgramResource::Control>::const_iterator iter = vec4Controls.begin(); iter != vec4Controls.end(); ++ iter) {
            const ShaderProgramResource::Control& entry = *iter;
            if(entry.name == "cascadeFars") {
                mCascadeFarsHandle = entry.handle;
            }
        }
        const std::vector<ShaderProgramResource::Control>& mat4Controls = mShaderProg->getUniformMat4s();
        for(std::vector<ShaderProgramResource::Control>::const_iterator iter = mat4Controls.begin(); iter != mat4Controls.end(); ++ iter) {
            const ShaderProgramResource::Control& entry = *iter;
            if(entry.name == "sunViewProjection0") {
                mSunViewProjHandle[0] = entry.handle;
            } else if(entry.name == "sunViewProjection1") {
                mSunViewProjHandle[1] = entry.handle;
            } else if(entry.name == "sunViewProjection2") {
                mSunViewProjHandle[2] = entry.handle;
            } else if(entry.name == "sunViewProjection3") {
                mSunViewProjHandle[3] = entry.handle;
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

    glEnableVertexAttribArray(mShaderProg->getPosAttrib());
    glVertexAttribPointer(mShaderProg->getPosAttrib(), 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (void*) (0 * sizeof(GLfloat)));
    glEnableVertexAttribArray(mMinimalShader->getPosAttrib());
    glVertexAttribPointer(mMinimalShader->getPosAttrib(), 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (void*) (0 * sizeof(GLfloat)));

    glBindVertexArray(0);
}
void SunLightModel::SharedResources::unload() {
    mShaderProg->drop();
    mMinimalShader->drop();
    
    glDeleteBuffers(1, &mDLightIbo);
    glDeleteBuffers(1, &mDLightVbo);
    glDeleteVertexArrays(1, &mDLightVao);
}
void SunLightModel::SharedResources::render(const Model::RenderPass& rendPass, const glm::mat4& modelMat, const glm::vec3& lightColor) {
    if(rendPass.type != Model::RenderPass::Type::GLOBAL_LIGHTS) {
        return;
    }
    
    glm::vec3 lightDirection = glm::vec3(modelMat * glm::vec4(0.0, 0.0, 1.0, 0.0));
    
    glUseProgram(mShaderProg->getHandle());
    
    mShaderProg->bindModelViewProjMatrices(modelMat, rendPass.viewMat, rendPass.projMat);
    
    glUniform2fv(mDiskHandle, 64, mKernels.disk);
    glUniform1fv(mNearPlaneHandle, 1, rendPass.cascadeBorders);
    glUniform3fv(mColorHandle, 1, glm::value_ptr(lightColor));
    glUniform3fv(mDirectionHandle, 1, glm::value_ptr(lightDirection));
    glUniform3fv(mCameraLocHandle, 1, glm::value_ptr(rendPass.camPos));
    glUniform4fv(mCascadeFarsHandle, 1, glm::value_ptr(glm::vec4(
        rendPass.cascadeBorders[1], 
        rendPass.cascadeBorders[2], 
        rendPass.cascadeBorders[3], 
        rendPass.cascadeBorders[4])));
    glUniform2fv(mNoiseScaleHandle, 1, glm::value_ptr(glm::vec2(1280.f / 8.f, 720.f / 8.f)));
    
    for(uint32_t i = 0; i < PGG_NUM_SUN_CASCADES; ++ i) {
        glUniformMatrix4fv(mSunViewProjHandle[i], 1, GL_FALSE, glm::value_ptr(rendPass.sunViewProjMatr[i]));
    
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, rendPass.sunDepthTexture[i]);
        glUniform1i(mSunDepthHandle[i], i);
    }
    
    glActiveTexture(GL_TEXTURE0 + 4);
    glBindTexture(GL_TEXTURE_2D, rendPass.normalTexture);
    glUniform1i(mNormalHandle, 4);
    
    glActiveTexture(GL_TEXTURE0 + 5);
    glBindTexture(GL_TEXTURE_2D, rendPass.depthStencilTexture);
    glUniform1i(mDepthHandle, 5);
    
    glActiveTexture(GL_TEXTURE0 + 6);
    glBindTexture(GL_TEXTURE_2D, mKernels.normalized2DNoiseTexture);
    glUniform1i(mNoiseTextureHandle, 6);

    glBindVertexArray(mDLightVao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glUseProgram(0);
}

void SunLightModel::setColor(const glm::vec3& color) {
    mColor = color;
}

SunLightModel::SunLightModel(glm::vec3 color)
: mColor(color) {
}
SunLightModel::~SunLightModel() {
}
void SunLightModel::load() {
    mSharedRes = SharedResources::getSharedInstance();
    mSharedRes->grab();
}
void SunLightModel::unload() {
    mSharedRes->drop();
}

void SunLightModel::render(const Model::RenderPass& rendPass, const glm::mat4& modelMat) {
    mSharedRes->render(rendPass, modelMat, mColor);
}


}

