#ifndef PGG_DIRECTIONALLIGHTMODEL_HPP
#define PGG_DIRECTIONALLIGHTMODEL_HPP

#include "Model.hpp"
#include "ShaderProgramResource.hpp"

namespace pgg {

class DirectionalLightModel : public Model {
public:
    class SharedResources : public ReferenceCounted {
    private:
        ShaderProgramResource* mMinimalShader;
        ShaderProgramResource* mShaderProg;
        GLuint mNormalHandle;
        GLuint mDirectionHandle;
        GLuint mColorHandle;
        
        GLuint mDLightVao;
        GLuint mDLightVbo;
        GLuint mDLightIbo;
        
        SharedResources();
        ~SharedResources();
        
    public:
        static SharedResources* getSharedInstance();
        
        void load();
        void unload();
        void render(const Model::RenderPassConfiguration& rendPass, const glm::mat4& modelMat, const glm::vec3& color);
    };
private:
    SharedResources* mSharedRes;
    
    glm::vec3 mColor;
public:
    DirectionalLightModel(glm::vec3 color = glm::vec3(1.0));
    virtual ~DirectionalLightModel();

    void load();
    void unload();

    void render(const Model::RenderPassConfiguration& rendPass, const glm::mat4& modelMat);
    
    void setColor(const glm::vec3& color);
};

}

#endif // PGG_DIRECTIONALLIGHTMODEL_HPP
