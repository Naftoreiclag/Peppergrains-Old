/*
   Copyright 2015-2016 James Fong
   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at
       http://www.apache.org/licenses/LICENSE-2.0
   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#ifndef PGG_ShaderProgramResource_HPP
#define PGG_ShaderProgramResource_HPP

#include <string>
#include <vector>

#include <OpenGLStuff.hpp>

#include "Resource.hpp"
#include "ShaderResource.hpp"

namespace pgg {

class ShaderProgramResource : public Resource {
public:
    struct Control {
        std::string name;
        GLuint handle;
    };

    const std::vector<Control>& getUniformSampler2Ds() const;
    const std::vector<Control>& getUniformVec2s() const;
    const std::vector<Control>& getUniformVec3s() const;
    const std::vector<Control>& getUniformVec4s() const;
    
    const std::vector<Control>& getInstancedSampler2Ds() const;
    const std::vector<Control>& getInstancedVec2s() const;
    const std::vector<Control>& getInstancedVec3s() const;
    const std::vector<Control>& getInstancedVec4s() const;

private:

    GLuint mShaderProg;
    bool mLoaded;

    std::vector<ShaderResource*> mLinkedShaders;

    std::vector<Control> mUniformSampler2Ds;
    std::vector<Control> mUniformVec2s;
    std::vector<Control> mUniformVec3s;
    std::vector<Control> mUniformVec4s;
    
    std::vector<Control> mInstancedSampler2Ds;
    std::vector<Control> mInstancedVec2s;
    std::vector<Control> mInstancedVec3s;
    std::vector<Control> mInstancedVec4s;

    /*
     * Model
     * View
     * Proj
     * ModelView
     * ViewProj
     * ModelViewProj
     * 
     * InvModel
     * InvView
     * InvProj
     * InvModelView
     * InvViewProj
     * InvModelViewProj
     * 
     * SunViewProj
     */
    
    // Note to self: do not name any member variables mMat
    
    bool mUseMMat;
    bool mUseVMat;
    bool mUsePMat;
    bool mUseMVMat;
    bool mUseVPMat;
    bool mUseMVPMat;
    bool mUseIMMat;
    bool mUseIVMat;
    bool mUseIPMat;
    bool mUseIMVMat;
    bool mUseIVPMat;
    bool mUseIMVPMat;
    GLuint mMMatUnif;
    GLuint mVMatUnif;
    GLuint mPMatUnif;
    GLuint mMVMatUnif;
    GLuint mVPMatUnif;
    GLuint mMVPMatUnif;
    GLuint mIMMatUnif;
    GLuint mIVMatUnif;
    GLuint mIPMatUnif;
    GLuint mIMVMatUnif;
    GLuint mIVPMatUnif;
    GLuint mIMVPMatUnif;
    
    bool mUseSunViewProjMatrix;
    GLuint mSunViewProjMatrixUnif;

    bool mUsePosAttrib;
    GLuint mPosAttrib;

    bool mUseColorAttrib;
    GLuint mColorAttrib;

    bool mUseUVAttrib;
    GLuint mUVAttrib;

    bool mUseNormalAttrib;
    GLuint mNormalAttrib;

    bool mUseTangentAttrib;
    GLuint mTangentAttrib;

    bool mUseBitangentAttrib;
    GLuint mBitangentAttrib;
    
    void loadError();
    void unloadError();
    bool mIsErrorResource;
    GLuint mErrorFragShaderHandle;
    GLuint mErrorVertShaderHandle;

public:
    ShaderProgramResource();
    virtual ~ShaderProgramResource();
    
    void bindModelViewProjMatrices(const glm::mat4& modelMat, const glm::mat4& viewMat, const glm::mat4& projMat) const;
    
    void load();
    void unload();
    
    GLuint getHandle() const;
    
    bool needsModelMatrix() const;
    bool needsViewMatrix() const;
    bool needsProjMatrix() const;
    bool needsModelViewMatrix() const;
    bool needsViewProjMatrix() const;
    bool needsModelViewProjMatrix() const;
    bool needsInvModelMatrix() const;
    bool needsInvViewMatrix() const;
    bool needsInvProjMatrix() const;
    bool needsInvModelViewMatrix() const;
    bool needsInvViewProjMatrix() const;
    bool needsInvModelViewProjMatrix() const;
    
    GLuint getModelMatrixUnif() const;
    GLuint getViewMatrixUnif() const;
    GLuint getProjMatrixUnif() const;
    GLuint getModelViewMatrixUnif() const;
    GLuint getViewProjMatrixUnif() const;
    GLuint getModelViewProjMatrixUnif() const;
    GLuint getInvModelMatrixUnif() const;
    GLuint getInvViewMatrixUnif() const;
    GLuint getInvProjMatrixUnif() const;
    GLuint getInvModelViewMatrixUnif() const;
    GLuint getInvViewProjMatrixUnif() const;
    GLuint getInvModelViewProjMatrixUnif() const;
    
    bool needsSunViewProjMatrix() const;
    bool needsPosAttrib() const;
    bool needsColorAttrib() const;
    bool needsUVAttrib() const;
    bool needsNormalAttrib() const;
    bool needsTangentAttrib() const;
    bool needsBitangentAttrib() const;
    GLuint getSunViewProjMatrixUnif() const;
    GLuint getPosAttrib() const;
    GLuint getColorAttrib() const;
    GLuint getUVAttrib() const;
    GLuint getNormalAttrib() const;
    GLuint getTangentAttrib() const;
    GLuint getBitangentAttrib() const;

};

}

#endif // ShaderProgramResource_HPP
