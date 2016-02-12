#ifndef MATERIALRESOURCE_HPP
#define MATERIALRESOURCE_HPP

#include <OpenGLStuff.hpp>

#include "Resource.hpp" // Base class: Resource
#include "ShaderProgramResource.hpp"
#include "TextureResource.hpp"

class MaterialResource : public Resource {
private:
    struct Sampler2DControl {
        GLuint handle;
        TextureResource* texture;
    };

    std::vector<Sampler2DControl> mSampler2Ds;

    bool mLoaded;
    ShaderProgramResource* mShaderProg;

public:
    MaterialResource();
    virtual ~MaterialResource();

public:
    bool load();
    bool unload();

    void bindTextures();

    const ShaderProgramResource* getShaderProg() const;
};

#endif // MATERIALRESOURCE_HPP
