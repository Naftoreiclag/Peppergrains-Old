#ifndef PGG_POINTLIGHTMODEL_HPP
#define PGG_POINTLIGHTMODEL_HPP

#include "Model.hpp"
#include "GeometryResource.hpp"
#include "ShaderProgramResource.hpp"

namespace pgg {

class PointLightModel : public Model {
private:
    GLuint mVertexArrayObject;
    
    ShaderProgramResource* mShaderProg;
    GLuint mNormalHandle;
    GLuint mDepthHandle;
    
    GeometryResource* mGeometry;
public:
    PointLightModel();
    virtual ~PointLightModel();

    void load();
    void unload();

    void render(const Model::RenderPassConfiguration& rendPass, const glm::mat4& modelMat);

};

}

#endif // PGG_POINTLIGHTMODEL_HPP
