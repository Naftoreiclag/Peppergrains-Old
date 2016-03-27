#include "PointLightModel.hpp"

#include <iostream>

#include "ResourceManager.hpp"

namespace pgg {

PointLightModel::PointLightModel() {
}

PointLightModel::~PointLightModel() {
}
void PointLightModel::load() {
    ResourceManager* resman = ResourceManager::getSingleton();
    mGeometry = resman->findGeometry("PointLightVolume.geometry");
    mShaderProg = resman->findShaderProgram("PointLightVolume.shaderProgram");
    mDebug = resman->findModel("adfgeriojfdkwrg");
    mGeometry->grab();
    mShaderProg->grab();
    mDebug->grab();
    
    std::cout << "Is errorrrrrrr: " << mShaderProg->isFallback() << std::endl;
    
    const std::vector<ShaderProgramResource::Control>& sampler2DControls = mShaderProg->getUniformSampler2Ds();
    for(std::vector<ShaderProgramResource::Control>::const_iterator iter = sampler2DControls.begin(); iter != sampler2DControls.end(); ++ iter) {
        const ShaderProgramResource::Control& entry = *iter;
        
        if(entry.name == "normal") {
            mNormalHandle = entry.handle;
        }
        else if(entry.name == "depth") {
            mDepthHandle = entry.handle;
        }
    }
    
    glGenVertexArrays(1, &mVertexArrayObject);
    glBindVertexArray(mVertexArrayObject);

    mGeometry->bindBuffers();
    
    if(mShaderProg->needsPosAttrib()) {
        mGeometry->enablePositionAttrib(mShaderProg->getPosAttrib());
    }
    
    glBindVertexArray(0);
    
}
void PointLightModel::unload() {
    mDebug->drop();
    mGeometry->drop();
    mShaderProg->drop();
}

void PointLightModel::render(const Model::RenderPassConfiguration& rendPass, const glm::mat4& modelMat) {
    if(rendPass.type != Model::RenderPassType::BRIGHT) {
        //mDebug->render(rendPass, modelMat);
        return;
    }
    /*
    Model::RenderPassConfiguration mod = rendPass;
    mod.type = Model::RenderPassType::GEOMETRY;
    mDebug->render(mod, modelMat);
    */
    
    glUseProgram(mShaderProg->getHandle());

    mShaderProg->bindModelViewProjMatrices(modelMat, rendPass.viewMat, rendPass.projMat);
    
    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_2D, rendPass.normalTexture);
    glUniform1i(mNormalHandle, 0);

    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_2D, rendPass.depthStencilTexture);
    glUniform1i(mDepthHandle, 1);

    glBindVertexArray(mVertexArrayObject);

    mGeometry->drawElements();

    glBindVertexArray(0);

    glUseProgram(0);
}


}

