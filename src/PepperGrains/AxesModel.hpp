#ifndef PGG_AXESMODEL_HPP
#define PGG_AXESMODEL_HPP

#include "Model.hpp"
#include "ShaderProgramResource.hpp"
#include "OpenGLStuff.hpp"

namespace pgg {

class AxesModel : public Model {
private:
    GLuint mVertexArrayObject;
    GLuint mVertexBufferObject;
    GLuint mIndexBufferObject;
    ShaderProgramResource* mShaderProg;
public:
    AxesModel();
    ~AxesModel();

    bool load();
    bool unload();

    void render(const glm::mat4& viewMat, const glm::mat4& projMat, const glm::mat4& modelMat);

};

}

#endif // PGG_AXESMODEL_HPP
