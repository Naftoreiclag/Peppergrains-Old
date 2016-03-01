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
    struct Sampler2DControl {
        std::string name;
        GLuint handle;
    };

    const std::vector<Sampler2DControl>& getSampler2Ds() const;

private:

    GLuint mShaderProg;
    bool mLoaded;

    std::vector<ShaderResource*> mLinkedShaders;

    std::vector<Sampler2DControl> mSampler2Ds;

    bool mUseModelMatrix;
    GLuint mModelMatrixUnif;

    bool mUseViewMatrix;
    GLuint mViewMatrixUnif;

    bool mUseProjMatrix;
    GLuint mProjMatrixUnif;

    bool mUsePosAttrib;
    GLuint mPosAttrib;

    bool mUseColorAttrib;
    GLuint mColorAttrib;

    bool mUseUVAttrib;
    GLuint mUVAttrib;

    bool mUseNormalAttrib;
    GLuint mNormalAttrib;

public:
    ShaderProgramResource();
    virtual ~ShaderProgramResource();
    
    bool load();
    bool unload();
    
    GLuint getHandle() const;
    bool needsModelMatrix() const;
    bool needsViewMatrix() const;
    bool needsProjMatrix() const;
    bool needsPosAttrib() const;
    bool needsColorAttrib() const;
    bool needsUVAttrib() const;
    bool needsNormalAttrib() const;
    GLuint getModelMatrixUnif() const;
    GLuint getViewMatrixUnif() const;
    GLuint getProjMatrixUnif() const;
    GLuint getPosAttrib() const;
    GLuint getColorAttrib() const;
    GLuint getUVAttrib() const;
    GLuint getNormalAttrib() const;

};

}

#endif // ShaderProgramResource_HPP
