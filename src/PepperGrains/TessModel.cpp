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

#include "TessModel.hpp"

#include <iostream>

#include "ResourceManager.hpp"

namespace pgg {

TessModel::TessModel() {}
TessModel::~TessModel() {}

void TessModel::load() {

    ResourceManager* resman = ResourceManager::getSingleton();
    mShaderProg = resman->findShaderProgram("Tessellate.shaderProgram");
    mShaderProg->grab();
    
    std::cout << "Is error: " << mShaderProg->isFallback() << std::endl;

    mVertexSize = 3;
    mNumVertices = 4;
    mNumQuads = 1;

    GLfloat vertices[mNumVertices * mVertexSize];
    GLuint indices[mNumQuads * 4];

    {
        uint32_t i = 0;
        for(uint32_t x = 0; x < 2; ++ x) {
            for(uint32_t z = 0; z < 2; ++ z) {
                vertices[i ++] = x;
                vertices[i ++] = 0.f;
                vertices[i ++] = z;
            }
        }
    }
    
    /*  01
     *  23
     */
    
    indices[0] = 0;
    indices[1] = 2;
    indices[2] = 3;
    indices[3] = 1;
    
    {
        const std::vector<ShaderProgramResource::Control>& floatControls = mShaderProg->getUniformFloats();
        for(std::vector<ShaderProgramResource::Control>::const_iterator iter = floatControls.begin(); iter != floatControls.end(); ++ iter) {
            const ShaderProgramResource::Control& entry = *iter;

            if(entry.name == "levelInner") {
                mInner = entry.handle;
            } else if(entry.name == "levelOuter") {
                mOuter = entry.handle;
            }
        }
    }

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
    glVertexAttribPointer(mShaderProg->getPosAttrib(), 3, GL_FLOAT, GL_FALSE, mVertexSize * sizeof(GLfloat), (void*) (0 * sizeof(GLfloat)));

    glBindVertexArray(0);
}
void TessModel::unload() {
    mShaderProg->drop();

    glDeleteVertexArrays(1, &mVertexArrayObject);
    glDeleteBuffers(1, &mVertexBufferObject);
    glDeleteBuffers(1, &mIndexBufferObject);

    delete this;
}
void TessModel::render(const Model::RenderPassConfiguration& rendPass, const glm::mat4& modelMat) {
    
    if(rendPass.type != RenderPassType::GEOMETRY && rendPass.type != RenderPassType::SHADOW) {
        return;
    }
    
    glUseProgram(mShaderProg->getHandle());

    mShaderProg->bindModelViewProjMatrices(modelMat, rendPass.viewMat, rendPass.projMat);

    glUniform1f(mInner, 3.f);
    glUniform1f(mOuter, 3.f);

    glBindVertexArray(mVertexArrayObject);

    glPatchParameteri(GL_PATCH_VERTICES, 4);
    glDrawElements(GL_PATCHES, mNumQuads * 4, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);

    glUseProgram(0);
}

}
