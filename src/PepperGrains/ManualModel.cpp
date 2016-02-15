#include "ManualModel.hpp"

#include <iostream>

#include "ResourceManager.hpp"

namespace pgg {

ManualModel::ManualModel() {

    GLfloat vertices[] = {
        0.f, 0.f, 0.f,
        2.f, 0.f, 0.f,
        0.f, 2.f, 0.f,
        2.f, 2.f, 0.f
    };
    GLuint indices[] = {
        0, 1, 2,
        1, 3, 2,
    };

    ResourceManager* resman = ResourceManager::getSingleton();

    mShaderProg = resman->findShaderProgram("Red.shaderProgram");

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
    glVertexAttribPointer(mShaderProg->getPosAttrib(), 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*) (0 * sizeof(GLfloat)));

    // Finished initalizing vertex array object, so unbind
    glBindVertexArray(0);


}

ManualModel::~ManualModel()
{
}

void ManualModel::render(const glm::mat4& viewMat, const glm::mat4& projMat, const glm::mat4& modelMat) {

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

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);

    glUseProgram(0);
}


}
