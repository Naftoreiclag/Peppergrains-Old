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
    mShaderProg = resman->findShaderProgram("Heightmap.shaderProgram");
    mHeightTexture = resman->findTexture("GrassHeight.texture");
    mShaderProg->grab();
    mHeightTexture->grab();
    
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
    
    /* Order of vertices in buffer
     *  02
     *  13
     */
    
    indices[0] = 0;
    indices[1] = 1;
    indices[2] = 3;
    indices[3] = 2;
    
    /* Order of indices in buffer
     *  03
     *  12
     */
    
    {
        const std::vector<ShaderProgramResource::Control>& vec3Controls = mShaderProg->getUniformVec3s();
        for(std::vector<ShaderProgramResource::Control>::const_iterator iter = vec3Controls.begin(); iter != vec3Controls.end(); ++ iter) {
            const ShaderProgramResource::Control& entry = *iter;

            if(entry.name == "camPos") {
                mCamPos = entry.handle;
                std::cout << "e" << std::endl;
                break;
            }
        }
        const std::vector<ShaderProgramResource::Control>& floatControls = mShaderProg->getUniformFloats();
        for(std::vector<ShaderProgramResource::Control>::const_iterator iter = floatControls.begin(); iter != floatControls.end(); ++ iter) {
            const ShaderProgramResource::Control& entry = *iter;

            if(entry.name == "maxTess") {
                mMaxTess = entry.handle;
                std::cout << "a" << std::endl;
            } else if(entry.name == "minTess") {
                mMinTess = entry.handle;
                std::cout << "b" << std::endl;
            } else if(entry.name == "maxDist") {
                mMaxDist = entry.handle;
                std::cout << "c" << std::endl;
            } else if(entry.name == "minDist") {
                mMinDist = entry.handle;
                std::cout << "d" << std::endl;
            }
        }
        
        const std::vector<ShaderProgramResource::Control>& sampler2DControls = mShaderProg->getUniformSampler2Ds();
        for(std::vector<ShaderProgramResource::Control>::const_iterator iter = sampler2DControls.begin(); iter != sampler2DControls.end(); ++ iter) {
            const ShaderProgramResource::Control& entry = *iter;

                std::cout << entry.name << std::endl;
            if(entry.name == "heightMap") {
                mHeightMap = entry.handle;
                std::cout << "asdf" << std::endl;
                break;
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
    mHeightTexture->drop();

    glDeleteVertexArrays(1, &mVertexArrayObject);
    glDeleteBuffers(1, &mVertexBufferObject);
    glDeleteBuffers(1, &mIndexBufferObject);

    delete this;
}
void TessModel::render(const Model::RenderPass& rendPass, const glm::mat4& modelMat) {
    
    if(rendPass.type != RenderPass::Type::GEOMETRY && rendPass.type != RenderPass::Type::SHADOW) {
        return;
    }
    
    glUseProgram(mShaderProg->getHandle());

    mShaderProg->bindModelViewProjMatrices(modelMat, rendPass.viewMat, rendPass.projMat);
    
    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_2D, mHeightTexture->getHandle());
    glUniform1i(mHeightMap, 0);

    glUniform3fv(mCamPos, 1, glm::value_ptr(rendPass.camPos));
    glUniform1f(mMinTess, 1.f);
    glUniform1f(mMaxTess, 32.f);
    glUniform1f(mMinDist, 0.5f);
    glUniform1f(mMaxDist, 10.f);

    glBindVertexArray(mVertexArrayObject);

    glPatchParameteri(GL_PATCH_VERTICES, 4);
    glDrawElements(GL_PATCHES, mNumQuads * 4, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);

    glUseProgram(0);
}

}
