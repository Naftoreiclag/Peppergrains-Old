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

#include <iostream>

#include "ResourceManager.hpp"

namespace pgg {

SunLightModel::SharedResources::SharedResources() { }
SunLightModel::SharedResources::~SharedResources() { }
SunLightModel::SharedResources* SunLightModel::SharedResources::getSharedInstance() {
    static SharedResources instance;
    return &instance;
}

void SunLightModel::SharedResources::load() {
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
            } else if(entry.name == "sunDepth") {
                mSunDepthHandle = entry.handle;
            }
        }
        const std::vector<ShaderProgramResource::Control>& vec3Controls = mShaderProg->getUniformVec3s();
        for(std::vector<ShaderProgramResource::Control>::const_iterator iter = vec3Controls.begin(); iter != vec3Controls.end(); ++ iter) {
            const ShaderProgramResource::Control& entry = *iter;
            if(entry.name == "direction") {
                mDirectionHandle = entry.handle;
            } else if(entry.name == "color") {
                mColorHandle = entry.handle;
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
void SunLightModel::SharedResources::render(const Model::RenderPassConfiguration& rendPass, const glm::mat4& modelMat, const glm::vec3& lightColor) {
    if(rendPass.type != Model::RenderPassType::GLOBAL_LIGHTS) {
        return;
    }
    
    glm::vec3 lightDirection = glm::vec3(modelMat * glm::vec4(0.0, 0.0, 1.0, 0.0));
    
    glUseProgram(mShaderProg->getHandle());
    
    mShaderProg->bindModelViewProjMatrices(modelMat, rendPass.viewMat, rendPass.projMat);
    
    glUniform3fv(mColorHandle, 1, glm::value_ptr(lightColor));
    glUniform3fv(mDirectionHandle, 1, glm::value_ptr(lightDirection));
    
    glUniformMatrix4fv(mShaderProg->getSunViewProjMatrixUnif(), 1, GL_FALSE, glm::value_ptr(rendPass.sunViewProjMatr));
    
    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_2D, rendPass.normalTexture);
    glUniform1i(mNormalHandle, 0);
    
    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_2D, rendPass.depthStencilTexture);
    glUniform1i(mDepthHandle, 1);
    
    glActiveTexture(GL_TEXTURE0 + 2);
    glBindTexture(GL_TEXTURE_2D, rendPass.sunDepthTexture);
    glUniform1i(mSunDepthHandle, 2);

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

void SunLightModel::render(const Model::RenderPassConfiguration& rendPass, const glm::mat4& modelMat) {
    mSharedRes->render(rendPass, modelMat, mColor);
}


}
