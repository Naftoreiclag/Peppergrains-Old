#include "AxesModel.hpp"

#include <iostream>

#include "ResourceManager.hpp"

namespace pgg
{

AxesModel::AxesModel()
{
}

AxesModel::~AxesModel()
{
}

bool AxesModel::load() {
    GLfloat vertices[] = {
        0.f, 0.1f, 0.1f, 1.f, 0.f, 0.f,
        0.f, 0.9f, 0.1f, 1.f, 0.f, 0.f,
        0.f, 0.9f, 0.9f, 1.f, 0.f, 0.f,
        0.f, 0.1f, 0.9f, 1.f, 0.f, 0.f,
        
        0.1f, 0.f, 0.1f, 0.f, 1.f, 0.f,
        0.9f, 0.f, 0.1f, 0.f, 1.f, 0.f,
        0.9f, 0.f, 0.9f, 0.f, 1.f, 0.f,
        0.1f, 0.f, 0.9f, 0.f, 1.f, 0.f,
        
        0.1f, 0.1f, 0.f, 0.f, 0.f, 1.f,
        0.9f, 0.1f, 0.f, 0.f, 0.f, 1.f,
        0.9f, 0.9f, 0.f, 0.f, 0.f, 1.f,
        0.1f, 0.9f, 0.f, 0.f, 0.f, 1.f
    };
    
    GLuint indices[36];
    
    for(uint32_t i = 0; i < 3; ++ i) {
        uint32_t o = i * 12;
        uint32_t f = i * 4;
        
        indices[o + 0] = f + 0;
        indices[o + 1] = f + 1;
        indices[o + 2] = f + 2;
        indices[o + 3] = f + 3;
        indices[o + 4] = f + 2;
        indices[o + 5] = f + 0;
        
        indices[o +  6] = indices[o + 0];
        indices[o +  7] = indices[o + 2];
        indices[o +  8] = indices[o + 1];
        indices[o +  9] = indices[o + 3];
        indices[o + 10] = indices[o + 5];
        indices[o + 11] = indices[o + 4];
    }
    

    ResourceManager* resman = ResourceManager::getSingleton();

    mShaderProg = resman->findShaderProgram("SimpleVertexColor.shaderProgram");
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
    glVertexAttribPointer(mShaderProg->getPosAttrib(), 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*) (0 * sizeof(GLfloat)));
    glEnableVertexAttribArray(mShaderProg->getColorAttrib());
    glVertexAttribPointer(mShaderProg->getColorAttrib(), 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*) (3 * sizeof(GLfloat)));

    glBindVertexArray(0);
    return true;
}
bool AxesModel::unload() {
    glDeleteBuffers(1, &mIndexBufferObject);
    glDeleteBuffers(1, &mVertexBufferObject);

    mShaderProg->drop();

    glDeleteVertexArrays(1, &mVertexArrayObject);

    delete this;
    return true;
}
void AxesModel::render(const glm::mat4& viewMat, const glm::mat4& projMat, const glm::mat4& modelMat) {

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

    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);

    glUseProgram(0);
}

}
