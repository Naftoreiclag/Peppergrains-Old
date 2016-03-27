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
    mMinimalShader = resman->findShaderProgram("MinimalWhite.shaderProgram");
    mGeometry->grab();
    mShaderProg->grab();
    mDebug->grab();
    mMinimalShader->grab();
    
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
    if(mMinimalShader->needsPosAttrib()) {
        mGeometry->enablePositionAttrib(mMinimalShader->getPosAttrib());
    }
    
    glBindVertexArray(0);
    
}
void PointLightModel::unload() {
    mDebug->drop();
    mGeometry->drop();
    mShaderProg->drop();
    mMinimalShader->drop();
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
    
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDrawBuffer(GL_NONE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);
    glDisable(GL_CULL_FACE);
    glClearStencil(0);
    glClear(GL_STENCIL_BUFFER_BIT);
    
    glStencilFunc(GL_ALWAYS, 0, 0);
    
    glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);
    glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
    
    glUseProgram(mMinimalShader->getHandle());
    mMinimalShader->bindModelViewProjMatrices(modelMat, rendPass.viewMat, rendPass.projMat);
    glBindVertexArray(mVertexArrayObject);
    mGeometry->drawElements();
    glBindVertexArray(0);
    glUseProgram(0);
    
    GLuint colorAttachments[] = {
        GL_COLOR_ATTACHMENT0
    };
    glDrawBuffers(1, colorAttachments);
    
    glStencilFunc(GL_NOTEQUAL, 0, 0xff);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_ONE, GL_ONE);
    
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

