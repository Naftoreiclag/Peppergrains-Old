#include "MaterialResource.hpp"

#include <cassert>
#include <fstream>

#include "json/json.h"

#include "ResourceManager.hpp"
#include "ShaderProgramResource.hpp"

MaterialResource::MaterialResource()
: mLoaded(false) {
}

MaterialResource::~MaterialResource() {
}

bool MaterialResource::load() {
    if(mLoaded) {
        return true;
    }

    ResourceManager* rmgr = ResourceManager::getSingleton();

    Json::Value matData;

    {
        std::ifstream loader(this->getFile().c_str());
        loader >> matData;
        loader.close();
    }

    mShaderProg = rmgr->findShaderProgram(matData["shaderProgram"].asString());
    mShaderProg->grab();

    // Find all Sampler2Ds
    {
        const Json::Value& sampler2Ds = matData["sampler2D"];
        const std::vector<ShaderProgramResource::Sampler2DControl> sampler2DControls = mShaderProg->getSampler2Ds();

        for(std::vector<ShaderProgramResource::Sampler2DControl>::const_iterator iter = sampler2DControls.begin(); iter != sampler2DControls.end(); ++ iter) {
            const ShaderProgramResource::Sampler2DControl& entry = *iter;

            const Json::Value& value = sampler2Ds[entry.name];

            if(!value.isNull()) {
                Sampler2DControl control;
                control.handle = entry.handle;
                control.texture = rmgr->findTexture(value.asString());
                control.texture->grab();
                mSampler2Ds.push_back(control);
            }
        }
    }

    mLoaded = true;
    return true;
}

bool MaterialResource::unload() {
    assert(mLoaded && "Attempted to unload material before loading it");

    mShaderProg->drop();

    for(std::vector<Sampler2DControl>::iterator iter = mSampler2Ds.begin(); iter != mSampler2Ds.end(); ++ iter) {
        Sampler2DControl& control = *iter;

        control.texture->drop();
    }

    mLoaded = false;
    return true;
}

void MaterialResource::bindTextures() {
    unsigned int index = 0;
    for(std::vector<Sampler2DControl>::iterator iter = mSampler2Ds.begin(); iter != mSampler2Ds.end(); ++ iter) {
        Sampler2DControl& control = *iter;
        glActiveTexture(GL_TEXTURE0 + index);
        glBindTexture(GL_TEXTURE_2D, control.texture->getHandle());
        glUniform1i(control.handle, index);
        ++ index;
    }
}

const ShaderProgramResource* MaterialResource::getShaderProg() const {
    return mShaderProg;
}
