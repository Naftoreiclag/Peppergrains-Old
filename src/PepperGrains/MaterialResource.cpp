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

#include "MaterialResource.hpp"

#include <cassert>
#include <fstream>

#include "json/json.h"

#include "ResourceManager.hpp"
#include "ShaderProgramResource.hpp"

namespace pgg {

MaterialResource::MaterialResource()
: mLoaded(false) {
}

MaterialResource::~MaterialResource() {
}

void MaterialResource::load() {
    assert(!mLoaded && "Attempted to load material that has already been loaded");

    ResourceManager* rmgr = ResourceManager::getSingleton();

    Json::Value matData;

    {
        std::ifstream loader(this->getFile().string().c_str());
        loader >> matData;
        loader.close();
    }

    mShaderProg = rmgr->findShaderProgram(matData["shaderProgram"].asString());
    mShaderProg->grab();

    // Find all Sampler2Ds
    {
        const Json::Value& sampler2Ds = matData["sampler2D"];

        if(!sampler2Ds.isNull()) {

            const std::vector<ShaderProgramResource::Sampler2DControl>& sampler2DControls = mShaderProg->getSampler2Ds();

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
    }

    mLoaded = true;
}

void MaterialResource::unload() {
    assert(mLoaded && "Attempted to unload material before loading it");

    mShaderProg->drop();

    for(std::vector<Sampler2DControl>::iterator iter = mSampler2Ds.begin(); iter != mSampler2Ds.end(); ++ iter) {
        Sampler2DControl& control = *iter;

        control.texture->drop();
    }

    mLoaded = false;
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

}
