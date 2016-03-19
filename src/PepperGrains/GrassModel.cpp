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

#include "GrassModel.hpp"
#include <iostream>

#include "ResourceManager.hpp"

namespace pgg {

GrassModel::GrassModel() {}
GrassModel::~GrassModel() {}

void GrassModel::load() {

    ResourceManager* resman = ResourceManager::getSingleton();
    mGeometry = resman->findGeometry("GrassBlade.geometry");
    mShaderProg = resman->findShaderProgram("GrassBlade.shaderProgram");
    mDiffuseTexture = resman->findTexture("Green.texture");
    mHeightTexture = resman->findTexture("GrassHeight.texture");
    mGeometry->grab();
    mShaderProg->grab();
    mDiffuseTexture->grab();
    mHeightTexture->grab();
    
    std::cout << "Is error: " << mShaderProg->isFallback() << std::endl;
    
    const std::vector<ShaderProgramResource::Control>& vec3Controls = mShaderProg->getInstancedVec3s();
    for(std::vector<ShaderProgramResource::Control>::const_iterator iter = vec3Controls.begin(); iter != vec3Controls.end(); ++ iter) {
        const ShaderProgramResource::Control& entry = *iter;
        mOffsetHandle = entry.handle;
        break;
    }
    const std::vector<ShaderProgramResource::Control>& vec2Controls = mShaderProg->getInstancedVec2s();
    for(std::vector<ShaderProgramResource::Control>::const_iterator iter = vec2Controls.begin(); iter != vec2Controls.end(); ++ iter) {
        const ShaderProgramResource::Control& entry = *iter;
        mColorHandle = entry.handle;
        break;
    }
    const std::vector<ShaderProgramResource::Control>& sampler2DControls = mShaderProg->getUniformSampler2Ds();
    for(std::vector<ShaderProgramResource::Control>::const_iterator iter = sampler2DControls.begin(); iter != sampler2DControls.end(); ++ iter) {
        const ShaderProgramResource::Control& entry = *iter;
        
        if(entry.name == "diffuseMap") {
            mDiffuseHandle = entry.handle;
            std::cout << "Diffuse " << mDiffuseHandle << std::endl;
        } else if(entry.name == "heightMap") {
            mHeightHandle = entry.handle;
            std::cout << "Height " << mHeightHandle << std::endl;
        }
    }
    
    glm::vec3 offsets[40000];
    glm::vec2 colors[40000];
    {
        float aX = -10;
        float aZ = -10;
        float bX = 10;
        float bZ = 10;
        
        uint32_t index = 0;
        for(uint32_t x = 0; x < 200; ++ x) {
            for(uint32_t z = 0; z < 200; ++ z) {
                float fx = x * ((bX - aX) / 200.f);
                float fz = z * ((bZ - aZ) / 200.f);
                fx += aX;
                fz += aZ;
                
                float u = x;
                float v = z;
                u /= 200.f;
                v /= 200.f;
                
                colors[index] = glm::vec2(u, v);
                offsets[index] = glm::vec3(fx, 0, fz);
                
                ++ index;
            }
        }
        
    }
    
    glGenBuffers(1, &mColorsBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, mColorsBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    glGenBuffers(1, &mOffsetsBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, mOffsetsBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(offsets), offsets, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenVertexArrays(1, &mVertexArrayObject);
    glBindVertexArray(mVertexArrayObject);

    mGeometry->bindBuffers();
    
    if(mShaderProg->needsPosAttrib()) {
        mGeometry->enablePositionAttrib(mShaderProg->getPosAttrib());
    }
    if(mShaderProg->needsColorAttrib()) {
        mGeometry->enableColorAttrib(mShaderProg->getColorAttrib());
    }
    if(mShaderProg->needsUVAttrib()) {
        mGeometry->enableUVAttrib(mShaderProg->getUVAttrib());
    }
    if(mShaderProg->needsNormalAttrib()) {
        mGeometry->enableNormalAttrib(mShaderProg->getNormalAttrib());
    }
    if(mShaderProg->needsTangentAttrib()) {
        mGeometry->enableTangentAttrib(mShaderProg->getTangentAttrib());
    }
    if(mShaderProg->needsBitangentAttrib()) {
        mGeometry->enableBitangentAttrib(mShaderProg->getBitangentAttrib());
    }
    
    glBindBuffer(GL_ARRAY_BUFFER, mColorsBuffer);
    
    glEnableVertexAttribArray(mColorHandle);
    glVertexAttribPointer(mColorHandle, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (void*) (0 * sizeof(GLfloat)));
    glVertexAttribDivisor(mColorHandle, 1);
    
    glBindBuffer(GL_ARRAY_BUFFER, mOffsetsBuffer);
    
    glEnableVertexAttribArray(mOffsetHandle);
    glVertexAttribPointer(mOffsetHandle, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*) (0 * sizeof(GLfloat)));
    glVertexAttribDivisor(mOffsetHandle, 1);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    //Do not unbind this!
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glBindVertexArray(0);
}
void GrassModel::unload() {
    mGeometry->drop();
    mShaderProg->drop();
    mDiffuseTexture->drop();
    mHeightTexture->drop();

    glDeleteVertexArrays(1, &mVertexArrayObject);

    delete this;
}
void GrassModel::render(const Model::RenderPassConfiguration& rendPass, const glm::mat4& modelMat) {
    
    if(rendPass.shadowMapping) {
        return;
    }
    
    glDisable(GL_CULL_FACE);
    
    glUseProgram(mShaderProg->getHandle());

    if(mShaderProg->needsModelMatrix()) {
        glUniformMatrix4fv(mShaderProg->getModelMatrixUnif(), 1, GL_FALSE, glm::value_ptr(modelMat));
    }
    if(mShaderProg->needsViewMatrix()) {
        glUniformMatrix4fv(mShaderProg->getViewMatrixUnif(), 1, GL_FALSE, glm::value_ptr(rendPass.viewMat));
    }
    if(mShaderProg->needsProjMatrix()) {
        glUniformMatrix4fv(mShaderProg->getProjMatrixUnif(), 1, GL_FALSE, glm::value_ptr(rendPass.projMat));
    }
    
    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_2D, mDiffuseTexture->getHandle());
    glUniform1i(mDiffuseHandle, 0);
    
    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_2D, mHeightTexture->getHandle());
    glUniform1i(mHeightHandle, 1);

    glBindVertexArray(mVertexArrayObject);

    mGeometry->drawElementsInstanced(40000);

    glBindVertexArray(0);

    glUseProgram(0);
    glEnable(GL_CULL_FACE);
}

}
