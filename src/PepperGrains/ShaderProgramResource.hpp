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

    const std::vector<Control>& getSampler2Ds() const;
    const std::vector<Control>& getVec3s() const;
    const std::vector<Control>& getVec4s() const;

private:

    GLuint mShaderProg;
    bool mLoaded;

    std::vector<ShaderResource*> mLinkedShaders;

    std::vector<Control> mSampler2Ds;
    std::vector<Control> mVec3s;
    std::vector<Control> mVec4s;

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
    
    bool mUseModelMatrix;
    GLuint mModelMatrixUnif;

    bool mUseViewMatrix;
    GLuint mViewMatrixUnif;

    bool mUseProjMatrix;
    GLuint mProjMatrixUnif;
    
    bool mUseInvViewProjMatrix;
    GLuint mInvViewProjMatrixUnif;
    
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
    
    void load();
    void unload();
    
    GLuint getHandle() const;
    bool needsModelMatrix() const;
    bool needsViewMatrix() const;
    bool needsProjMatrix() const;
    bool needsInvViewProjMatrix() const;
    bool needsSunViewProjMatrix() const;
    bool needsPosAttrib() const;
    bool needsColorAttrib() const;
    bool needsUVAttrib() const;
    bool needsNormalAttrib() const;
    bool needsTangentAttrib() const;
    bool needsBitangentAttrib() const;
    GLuint getModelMatrixUnif() const;
    GLuint getViewMatrixUnif() const;
    GLuint getProjMatrixUnif() const;
    GLuint getInvViewProjMatrixUnif() const;
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
