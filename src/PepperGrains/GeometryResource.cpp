#include "GeometryResource.hpp"

#include <fstream>
#include <iostream>

#include <OpenGLStuff.hpp>

namespace pgg {

GeometryResource::GeometryResource()
: mLoaded(false) {
}

GeometryResource::~GeometryResource() {
}

uint32_t readU32(std::ifstream& input) {
    uint32_t value;
    input.read((char*) &value, sizeof value);
    return value;
}

bool readBool(std::ifstream& input) {
    char value;
    input.read(&value, sizeof value);
    return value != 0;
}

uint16_t readU16(std::ifstream& input) {
    uint16_t value;
    input.read((char*) &value, sizeof value);
    return value;
}

uint8_t readU8(std::ifstream& input) {
    char value;
    input.read(&value, sizeof value);
    return value;
}

float readF32(std::ifstream& input) {
    float value;
    input.read((char*) &value, sizeof value);
    return value;
}

bool GeometryResource::load() {
    if(mLoaded) {
        return true;
    }

    std::ifstream input(this->getFile().c_str(), std::ios::in | std::ios::binary);

    mUsePositions = readBool(input);
    mUseColor = readBool(input);
    mUseUV = readBool(input);
    mUseNormals = readBool(input);

    mNumVertices = readU32(input);
    mNumTriangles = readU32(input);

    mPositionOff = 0;
    mColorOff = mPositionOff + (mUsePositions ? 3 : 0);
    mTextureOff = mColorOff + (mUseColor ? 3 : 0);
    mNormalOff = mTextureOff + (mUseUV ? 2 : 0);
    mSizeVertices = mNormalOff + (mUseNormals ? 3 : 0);

    GLfloat vertices[mNumVertices * mSizeVertices];

    for(uint32_t i = 0; i < mNumVertices; ++ i) {
        if(mUsePositions) {
            vertices[(i * mSizeVertices) + mPositionOff + 0] = readF32(input);
            vertices[(i * mSizeVertices) + mPositionOff + 1] = readF32(input);
            vertices[(i * mSizeVertices) + mPositionOff + 2] = readF32(input);
        }
        if(mUseColor) {
            vertices[(i * mSizeVertices) + mColorOff + 0] = readF32(input);
            vertices[(i * mSizeVertices) + mColorOff + 1] = readF32(input);
            vertices[(i * mSizeVertices) + mColorOff + 2] = readF32(input);
        }
        if(mUseUV) {
            vertices[(i * mSizeVertices) + mTextureOff + 0] = readF32(input);
            vertices[(i * mSizeVertices) + mTextureOff + 1] = readF32(input);
        }
        if(mUseNormals) {
            vertices[(i * mSizeVertices) + mNormalOff + 0] = readF32(input);
            vertices[(i * mSizeVertices) + mNormalOff + 1] = readF32(input);
            vertices[(i * mSizeVertices) + mNormalOff + 2] = readF32(input);
        }
    }

    GLuint indices[mNumTriangles * 3];
    for(uint32_t i = 0; i < mNumTriangles; ++ i) {
        indices[(i * 3) + 0] = readU32(input);
        indices[(i * 3) + 1] = readU32(input);
        indices[(i * 3) + 2] = readU32(input);
    }

    input.close();


    glGenBuffers(1, &mVertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, mVertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &mIndexBufferObject);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBufferObject);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    return true;
}

bool GeometryResource::unload() {
    glDeleteBuffers(1, &mIndexBufferObject);
    glDeleteBuffers(1, &mVertexBufferObject);

    mLoaded = false;
    return true;
}

void GeometryResource::render() {
    glDrawElements(GL_TRIANGLES, mNumTriangles * 3, GL_UNSIGNED_INT, 0);
}

void GeometryResource::bindBuffers() {
    glBindBuffer(GL_ARRAY_BUFFER, mVertexBufferObject);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBufferObject);
}
void GeometryResource::enablePositionAttrib(GLuint posAttrib) {
    if(mUsePositions) {
        glEnableVertexAttribArray(posAttrib);
        glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, mSizeVertices * sizeof(GLfloat), (void*) (mPositionOff * sizeof(GLfloat)));
    }
}
void GeometryResource::enableColorAttrib(GLuint colorAttrib) {
    if(mUseColor) {
        glEnableVertexAttribArray(colorAttrib);
        glVertexAttribPointer(colorAttrib, 3, GL_FLOAT, GL_FALSE, mSizeVertices * sizeof(GLfloat), (void*) (mColorOff * sizeof(GLfloat)));
    }
}
void GeometryResource::enableUVAttrib(GLuint textureAttrib) {
    if(mUseUV) {
        glEnableVertexAttribArray(textureAttrib);
        glVertexAttribPointer(textureAttrib, 2, GL_FLOAT, GL_FALSE, mSizeVertices * sizeof(GLfloat), (void*) (mTextureOff * sizeof(GLfloat)));
    }
}
void GeometryResource::enableNormalAttrib(GLuint normalAttrib) {
    if(mUseNormals) {
        glEnableVertexAttribArray(normalAttrib);
        glVertexAttribPointer(normalAttrib, 3, GL_FLOAT, GL_FALSE, mSizeVertices * sizeof(GLfloat), (void*) (mNormalOff * sizeof(GLfloat)));
    }
}

}
