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

namespace pgg {

TextModel::TextModel(FontResource* font, std::string text)
: mFont(font)
, mText(text) { }
TextModel::~TextModel() { }

bool TextModel::load() {

    mNumGlyphs = mText.length();

    uint32_t vertexLength = 4;
    uint32_t glyphLength = 4 * vertexLength;

    float width = 10;
    float height = 10;

    GLfloat vertices[mNumGlyphs * glyphLength];
    GLuint indices[mNumGlyphs * 6];

    mFont->grab();

    {
        uint32_t index = 0;
        for(std::string::iterator it = mText.begin(); it != mText.end(); ++ it) {

            for(uint32_t iy = 0; iy < 2; ++ iy) {
                for(uint32_t ix = 0; ix < 2; ++ ix) {
                    uint32_t romeo = (index * glyphLength) + (ix * vertexLength) + (iy * vertexLength * 2);

                    vertices[romeo + 0] = ix == 0 ? index * width : (index * width) + width;
                    vertices[romeo + 1] = iy == 0 ? 0.f : height;
                    vertices[romeo + 2] = ix == 0 ? 0.f : 1.f;
                    vertices[romeo + 3] = iy == 0 ? 1.f : 0.f;
                }
            }

            indices[(index * 6) + 0] = 0;
            indices[(index * 6) + 1] = 1;
            indices[(index * 6) + 2] = 2;
            indices[(index * 6) + 3] = 1;
            indices[(index * 6) + 4] = 3;
            indices[(index * 6) + 5] = 2;

            ++ index;
        }
    }
    //

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

    glEnableVertexAttribArray(mShaderProg->getPosAttrib());

    // Uses 2D coordinates
    glVertexAttribPointer(mShaderProg->getPosAttrib(), 2, GL_FLOAT, GL_FALSE, vertexLength * sizeof(GLfloat), (void*) (0 * sizeof(GLfloat)));

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

    glBindVertexArray(mVertexArrayObject);

    glDrawElements(GL_TRIANGLES, mNumGlyphs * 6, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);

    glUseProgram(0);
}

}
