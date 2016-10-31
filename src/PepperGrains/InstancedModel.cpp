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

#include "InstancedModel.hpp"

#include <iostream>

#include "ResourceManager.hpp"

namespace pgg {

InstancedModel::InstancedModel() {}
InstancedModel::~InstancedModel() {}

void InstancedModel::load() {

    ResourceManager* resman = ResourceManager::getSingleton();
    mGeometry = resman->findGeometry("Error.geometry");
    mShaderProg = resman->findShaderProgram("TestInstanced.shaderProgram");
    mGeometry->grab();
    mShaderProg->grab();
    
    std::cout << "Is error: " << mShaderProg->isFallback() << std::endl;
    
    const std::vector<ShaderProgramResource::Control>& vec3Controls = mShaderProg->getInstancedVec3s();
    for(std::vector<ShaderProgramResource::Control>::const_iterator iter = vec3Controls.begin(); iter != vec3Controls.end(); ++ iter) {
        const ShaderProgramResource::Control& entry = *iter;

        mOffsetHandle = entry.handle;
        std::cout << "found" << std::endl;
        std::cout << entry.name << std::endl;
        std::cout << entry.handle << std::endl;

        // (Might want other samplers in the future)
        break;
    }
    
    glm::vec3 offsets[160000];
    {
        uint32_t index = 0;
        for(uint32_t x = 0; x < 400; ++ x) {
            for(uint32_t z = 0; z < 400; ++ z) {
                float fx = x * 4;
                float fz = z * 4;
                fx -= 800;
                fz -= 800;
                
                offsets[index ++] = glm::vec3(fx, 10, fz);
            }
        }
        
    }
    
    glGenBuffers(1, &mInstanceBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, mInstanceBuffer);
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
    
    glBindBuffer(GL_ARRAY_BUFFER, mInstanceBuffer);
    
    glEnableVertexAttribArray(mOffsetHandle);
    glVertexAttribPointer(mOffsetHandle, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*) (0 * sizeof(GLfloat)));
    glVertexAttribDivisor(mOffsetHandle, 1);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    //Do not unbind this!
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glBindVertexArray(0);
}
void InstancedModel::unload() {
    mGeometry->drop();
    mShaderProg->drop();

    glDeleteVertexArrays(1, &mVertexArrayObject);

    delete this;
}
void InstancedModel::render(const Renderable::Pass& rendPass, const glm::mat4& modelMat) {
    
    if(rendPass.type != Renderable::Pass::Type::GEOMETRY && rendPass.type != Renderable::Pass::Type::SHADOW) {
        return;
    }
    
    glUseProgram(mShaderProg->getHandle());

    mShaderProg->bindModelViewProjMatrices(modelMat, rendPass.viewMat, rendPass.projMat);

    glBindVertexArray(mVertexArrayObject);

    mGeometry->drawElementsInstanced(160000);

    glBindVertexArray(0);

    glUseProgram(0);
}

}
