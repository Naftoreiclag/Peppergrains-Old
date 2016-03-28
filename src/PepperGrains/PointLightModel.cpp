#include "PointLightModel.hpp"

#include <cmath>
#include <iostream>

#include "ResourceManager.hpp"

namespace pgg {
    
void PointLightModel::calcAttenFactors() {
    
    float red = mBrightness.r;
    float green = mBrightness.g;
    float blue = mBrightness.b;
    
    float color = red > green ? red : (green > blue ? green : blue);
    
    
    float minLight = 0.002f;
    mVolumeRadius = mRadius * std::sqrt(color / minLight);
    
    std::cout << "color: " << color << std::endl;
    std::cout << "minimum light: " << minLight << std::endl;
    std::cout << "radius: " << mRadius << std::endl;
    std::cout << "volRadius: " << mVolumeRadius << std::endl;
}

void PointLightModel::setBrightness(glm::vec3 brightness, float radius) {
    mBrightness = brightness;
    mRadius = radius;
    calcAttenFactors();
}

PointLightModel::PointLightModel(glm::vec3 brightness, float radius)
: mBrightness(brightness)
, mRadius(radius) {
    calcAttenFactors();
}

PointLightModel::~PointLightModel() {
}
void PointLightModel::load() {
    ResourceManager* resman = ResourceManager::getSingleton();
    mGeometry = resman->findGeometry("PointLightVolume.geometry");
    mShaderProg = resman->findShaderProgram("PointLightVolume.shaderProgram");
    mMinimalShader = resman->findShaderProgram("PointLightStencil.shaderProgram");
    mGeometry->grab();
    mShaderProg->grab();
    mMinimalShader->grab();
    
    {
        const std::vector<ShaderProgramResource::Control>& floatControls = mMinimalShader->getUniformFloats();
        for(std::vector<ShaderProgramResource::Control>::const_iterator iter = floatControls.begin(); iter != floatControls.end(); ++ iter) {
            const ShaderProgramResource::Control& entry = *iter;
            if(entry.name == "volumeRadius") {
                mStencilVolumeRadiusHandle = entry.handle;
            }
        }
    }
    {
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
        const std::vector<ShaderProgramResource::Control>& vec3Controls = mShaderProg->getUniformVec3s();
        for(std::vector<ShaderProgramResource::Control>::const_iterator iter = vec3Controls.begin(); iter != vec3Controls.end(); ++ iter) {
            const ShaderProgramResource::Control& entry = *iter;
            if(entry.name == "position") {
                mPositionHandle = entry.handle;
            }
            else if(entry.name == "color") {
                mColorHandle = entry.handle;
            }
        }
        const std::vector<ShaderProgramResource::Control>& floatControls = mShaderProg->getUniformFloats();
        for(std::vector<ShaderProgramResource::Control>::const_iterator iter = floatControls.begin(); iter != floatControls.end(); ++ iter) {
            const ShaderProgramResource::Control& entry = *iter;
            if(entry.name == "radius") {
                mRadiusHandle = entry.handle;
                std::cout << "aaa" << mRadiusHandle << std::endl;
            }
            else if(entry.name == "volumeRadius") {
                mVolumeRadiusHandle = entry.handle;
                std::cout << "ccc" << mVolumeRadiusHandle << std::endl;
            }
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
    mGeometry->drop();
    mShaderProg->drop();
    mMinimalShader->drop();
}

void PointLightModel::render(const Model::RenderPassConfiguration& rendPass, const glm::mat4& modelMat) {
    if(rendPass.type != Model::RenderPassType::BRIGHT) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glUseProgram(mMinimalShader->getHandle());
        mMinimalShader->bindModelViewProjMatrices(modelMat, rendPass.viewMat, rendPass.projMat);
        glUniform1f(mStencilVolumeRadiusHandle, mRadius);
        glBindVertexArray(mVertexArrayObject);
        mGeometry->drawElements();
        glBindVertexArray(0);
        glUseProgram(mMinimalShader->getHandle());
        glUniform1f(mStencilVolumeRadiusHandle, mVolumeRadius);
        glBindVertexArray(mVertexArrayObject);
        mGeometry->drawElements();
        glBindVertexArray(0);
        glUseProgram(0);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        
        return;
    }
    
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDrawBuffer(GL_NONE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);
    glDisable(GL_CULL_FACE);
    glClearStencil(1);
    glClear(GL_STENCIL_BUFFER_BIT);
    
    glStencilFunc(GL_ALWAYS, 0, 0);
    
    glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_INCR, GL_KEEP);
    glStencilOpSeparate(GL_BACK, GL_KEEP, GL_DECR, GL_KEEP);
    
    glUseProgram(mMinimalShader->getHandle());
    mMinimalShader->bindModelViewProjMatrices(modelMat, rendPass.viewMat, rendPass.projMat);
    glUniform1f(mStencilVolumeRadiusHandle, mVolumeRadius);
    glBindVertexArray(mVertexArrayObject);
    mGeometry->drawElements();
    glBindVertexArray(0);
    glUseProgram(0);
    
    GLuint colorAttachments[] = {
        GL_COLOR_ATTACHMENT0
    };
    glDrawBuffers(1, colorAttachments);
    
    // Only keep pixels for which this conditional is true
    glStencilFunc(GL_EQUAL, 0, 0xff);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_ONE, GL_ONE);
    
    glUseProgram(mShaderProg->getHandle());

    glm::vec3 lightPosition = glm::vec3(modelMat[3]);

    mShaderProg->bindModelViewProjMatrices(modelMat, rendPass.viewMat, rendPass.projMat);
    glUniform3fv(mColorHandle, 1, glm::value_ptr(mBrightness));
    glUniform3fv(mPositionHandle, 1, glm::value_ptr(lightPosition));
    glUniform1f(mRadiusHandle, mRadius);
    glUniform1f(mVolumeRadiusHandle, mVolumeRadius);
    
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

