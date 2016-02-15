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

#include "TextModel.hpp"


#include "ResourceManager.hpp"

namespace pgg {

TextModel::TextModel(FontResource* font, std::string text)
: mFont(font)
, mText(text) { }
TextModel::~TextModel() { }

bool TextModel::load() {

    mNumGlyphs = mText.length();

    uint32_t vertexLength = 4;
    uint32_t glyphLength = 4 * vertexLength;

    float width = 40;
    float height = 40;

    GLfloat vertices[mNumGlyphs * glyphLength];
    GLuint indices[mNumGlyphs * 6];

    mFont->grab();

    {
        uint32_t index = 0;
        float accX = 0.f;
        for(std::string::const_iterator it = mText.begin(); it != mText.end(); ++ it) {

            const char& glyphChar = *it;

            /*  C       D
             *
             *
             *  A       B
             */

            uint32_t glyphId = (uint32_t) glyphChar;
            float uvX = ( (float) (glyphId % 16) ) / 16.f;
            float uvY = ( (float) ((glyphId - (glyphId % 16)) / 16) ) / 16.f;
            float uvW = 1.f / 16.f;
            float uvH = 1.f / 16.f;

            for(uint32_t iy = 0; iy < 2; ++ iy) {
                for(uint32_t ix = 0; ix < 2; ++ ix) {
                    uint32_t romeo = (index * glyphLength) + (ix * vertexLength) + (iy * vertexLength * 2);

                    vertices[romeo + 0] = ix == 0 ? accX : accX + width;
                    vertices[romeo + 1] = iy == 0 ? 0.f : height;
                    vertices[romeo + 2] = ix == 0 ? uvX : uvX + uvW;
                    vertices[romeo + 3] = iy == 0 ? uvY + uvH : uvY;
                }
            }

            accX += mFont->mGlyphs[glyphId].width * width;

            indices[(index * 6) + 0] = (index * 4) + 0;
            indices[(index * 6) + 1] = (index * 4) + 1;
            indices[(index * 6) + 2] = (index * 4) + 2;
            indices[(index * 6) + 3] = (index * 4) + 1;
            indices[(index * 6) + 4] = (index * 4) + 3;
            indices[(index * 6) + 5] = (index * 4) + 2;

            ++ index;
        }
    }
    //

    ResourceManager* resman = ResourceManager::getSingleton();

    mShaderProg = resman->findShaderProgram("Font.shaderProgram");
    mShaderProg->grab();

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


    // (Uses 2D coordinates)
    glEnableVertexAttribArray(mShaderProg->getPosAttrib());
    glVertexAttribPointer(mShaderProg->getPosAttrib(), 2, GL_FLOAT, GL_FALSE, vertexLength * sizeof(GLfloat), (void*) (0 * sizeof(GLfloat)));
    glEnableVertexAttribArray(mShaderProg->getUVAttrib());
    glVertexAttribPointer(mShaderProg->getUVAttrib(), 2, GL_FLOAT, GL_FALSE, vertexLength * sizeof(GLfloat), (void*) (2 * sizeof(GLfloat)));

    glBindVertexArray(0);

    return true;
}

bool TextModel::unload() {
    glDeleteBuffers(1, &mIndexBufferObject);
    glDeleteBuffers(1, &mVertexBufferObject);

    mShaderProg->drop();

    glDeleteVertexArrays(1, &mVertexArrayObject);

    delete this;
    return true;
}

void TextModel::render(const glm::mat4& viewMat, const glm::mat4& projMat, const glm::mat4& modelMat) {

    glUseProgram(mShaderProg->getHandle());

    if(mShaderProg->needsModelMatrix()) {
        glUniformMatrix4fv(mShaderProg->getModelMatrixUnif(), 1, GL_FALSE, glm::value_ptr(modelMat));
    }
    if(mShaderProg->needsViewMatrix()) {
        glUniformMatrix4fv(mShaderProg->getViewMatrixUnif(), 1, GL_FALSE, glm::value_ptr(viewMat));
    }
    if(mShaderProg->needsProjMatrix()) {
        glUniformMatrix4fv(mShaderProg->getProjMatrixUnif(), 1, GL_FALSE, glm::value_ptr(projMat));
    }

    mFont->bindTextures();

    glBindVertexArray(mVertexArrayObject);

    glDrawElements(GL_TRIANGLES, mNumGlyphs * 6, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);

    glUseProgram(0);
}

}
