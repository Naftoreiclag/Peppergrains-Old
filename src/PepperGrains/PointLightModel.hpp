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
    GLuint mRadiusHandle;
    GLuint mVolumeRadiusHandle;
    GLuint mStencilVolumeRadiusHandle;
    GeometryResource* mGeometry;
    
    GLfloat mVolumeRadius;
    GLfloat mRadius;
    
    glm::vec3 mBrightness;
    
    void calcAttenFactors();
public:
    PointLightModel(glm::vec3 brightness = glm::vec3(1.0), float radius = 1.f);
    virtual ~PointLightModel();

    void load();
    void unload();

    void render(const Model::RenderPassConfiguration& rendPass, const glm::mat4& modelMat);
    
    void setBrightness(glm::vec3 brightness, float radius);
};

}

#endif // PGG_POINTLIGHTMODEL_HPP
