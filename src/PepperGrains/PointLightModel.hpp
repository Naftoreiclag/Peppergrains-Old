#ifndef PGG_POINTLIGHTMODEL_HPP
#define PGG_POINTLIGHTMODEL_HPP

#include "Model.hpp"
#include "GeometryResource.hpp"
#include "ShaderProgramResource.hpp"

namespace pgg {

class PointLightModel : public Model {
private:
    GLuint mVertexArrayObject;
    ShaderProgramResource* mMinimalShader;
    ShaderProgramResource* mShaderProg;
    GLuint mNormalHandle;
    GLuint mDepthHandle;
    GLuint mPositionHandle;
    GLuint mColorHandle;
    GLuint mLinearHandle;
    GLuint mQuadrHandle;
    GLuint mRadiusHandle;
    GLuint mStencilRadiusHandle;
    GeometryResource* mGeometry;
    
    GLfloat mVolumeRadius;
    GLfloat mAttenLinear;
    GLfloat mAttenQuadr;
    
    glm::vec3 mBrightness;
    
    void calcAttenFactors();
public:
    PointLightModel(glm::vec3 brightness = glm::vec3(1.0));
    virtual ~PointLightModel();

    void load();
    void unload();

    void render(const Model::RenderPassConfiguration& rendPass, const glm::mat4& modelMat);
    
    void setBrightness(glm::vec3 brightness);
};

}

#endif // PGG_POINTLIGHTMODEL_HPP
