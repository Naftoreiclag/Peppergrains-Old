#include "PointLightModel.hpp"

#include <cmath>
#include <iostream>

#include "ResourceManager.hpp"

namespace pgg {
    
void PointLightModel::calcAttenFactors() {
    
    float red = mBrightness.r;
    float green = mBrightness.g;
    float blue = mBrightness.b;
    
    float range = (red > green ? red : (green > blue ? green : blue)) * 5.0f;
    
    float linear = 4.0f / range;
    float quadr = 80.0f / (range * range);
    
    float minLight = 6.f / 255.f;
    
    float radius = (std::sqrt((linear * linear) - (4.f * quadr * (1.f - (range / minLight)))) - linear) / (2.f * quadr);
    
    mAttenLinear = linear;
    mAttenQuadr = quadr;
    mVolumeRadius = radius;
    
    std::cout << "Range " << range << std::endl;
    std::cout << "Linear " << linear << std::endl;
    std::cout << "Quadr " << quadr << std::endl;
    std::cout << "Radius " << radius << std::endl;
}

void PointLightModel::setBrightness(glm::vec3 brightness) {
    mBrightness = brightness;
    calcAttenFactors();
}

PointLightModel::PointLightModel(glm::vec3 brightness)
: mBrightness(brightness) {
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
            if(entry.name == "radius") {
                mStencilRadiusHandle = entry.handle;
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
            if(entry.name == "linear") {
                mLinearHandle = entry.handle;
                std::cout << "aaa" << mLinearHandle << std::endl;
            }
            else if(entry.name == "quadratic") {
                mQuadrHandle = entry.handle;
                std::cout << "bbb" << mQuadrHandle << std::endl;
            }
            else if(entry.name == "radius") {
                mRadiusHandle = entry.handle;
                std::cout << "ccc" << mRadiusHandle << std::endl;
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
    glUniform1f(mStencilRadiusHandle, mVolumeRadius);
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
    glUniform1f(mLinearHandle, mAttenLinear);
    glUniform1f(mQuadrHandle, mAttenQuadr);
    glUniform1f(mRadiusHandle, mVolumeRadius);
    
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

