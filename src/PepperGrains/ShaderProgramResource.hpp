#ifndef ShaderProgramResource_HPP
#define ShaderProgramResource_HPP

#include <string>
#include <vector>

#include <OpenGLStuff.hpp>

#include "Resource.hpp"
#include "ShaderResource.hpp"

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
    std::string mFragOutSymbol;

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

#endif // ShaderProgramResource_HPP
