#ifndef PGG_POINTLIGHTMODEL_HPP
#define PGG_POINTLIGHTMODEL_HPP

#include "Model.hpp"
#include "GeometryResource.hpp"
#include "ShaderProgramResource.hpp"

namespace pgg {

class PointLightModel : public Model {
public:
    class SharedResources : public ReferenceCounted {
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
        GLuint mStencilPositionHandle;
        GeometryResource* mGeometry;
        
        SharedResources();
        ~SharedResources();
        
    public:
        static SharedResources* getSharedInstance();
        
        void load();
        void unload();
        void render(const Model::RenderPassConfiguration& rendPass, const glm::mat4& modelMat, const glm::vec3& color, const GLfloat& radius, const GLfloat& volumeRadius);
    };
private:
    
    SharedResources* mSharedRes;
    
    glm::vec3 mColor;
    GLfloat mRadius;
    GLfloat mVolumeRadius;
    
    static float calcVolumeRadius(const glm::vec3& color, const float& radius);
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
