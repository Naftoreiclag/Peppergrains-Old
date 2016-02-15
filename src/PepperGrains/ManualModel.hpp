#ifndef PGG_MANUALMODEL_HPP
#define PGG_MANUALMODEL_HPP

#include "ShaderProgramResource.hpp"
#include "OpenGLStuff.hpp"

namespace pgg {

class ManualModel {
private:
    GLuint mVertexArrayObject;
    GLuint mVertexBufferObject;
    GLuint mIndexBufferObject;
    ShaderProgramResource* mShaderProg;
public:
    ManualModel();
    virtual ~ManualModel();

    void render(const glm::mat4& viewMat, const glm::mat4& projMat, const glm::mat4& modelMat);

};

}

#endif // PGG_MANUALMODEL_HPP
