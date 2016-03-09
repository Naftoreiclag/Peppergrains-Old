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
#include <iostream>
#include <fstream>

#include "json/json.h"

#include "ResourceManager.hpp"
#include "ShaderProgramResource.hpp"

namespace pgg {

MaterialResource::MaterialResource()
: mLoaded(false)
, mIsErrorResource(false) {
}

MaterialResource::~MaterialResource() {
}

void MaterialResource::loadError() {
    assert(!mLoaded && "Attempted to load material that has already been loaded");
    
    ResourceManager* rmgr = ResourceManager::getSingleton();
    
    mShaderProg = rmgr->getFallbackShaderProgram();
    mShaderProg->grab();
    
    // Iterate through all the Sampler2Ds that the shader program requests
    const std::vector<ShaderProgramResource::Control>& sampler2DControls = mShaderProg->getSampler2Ds();
    for(std::vector<ShaderProgramResource::Control>::const_iterator iter = sampler2DControls.begin(); iter != sampler2DControls.end(); ++ iter) {
        const ShaderProgramResource::Control& entry = *iter;
        
        Sampler2DInput input;
        input.handle = entry.handle;
        input.texture = rmgr->getFallbackTexture();
        input.texture->grab();
        mSampler2Ds.push_back(input);
    }
    
    std::cout << "Hello Errors" << std::endl;
    
    mLoaded = true;
    mIsErrorResource = true;
}

void MaterialResource::unloadError() {
    assert(mLoaded && "Attempted to unload material before loading it");
    
    mShaderProg->drop();

    for(std::vector<Sampler2DInput>::iterator iter = mSampler2Ds.begin(); iter != mSampler2Ds.end(); ++ iter) {
        Sampler2DInput& input = *iter;

        input.texture->drop();
    }
    
    mLoaded = false;
    mIsErrorResource = false;
}

void MaterialResource::load() {
    assert(!mLoaded && "Attempted to load material that has already been loaded");

    if(this->isFallback()) {
        loadError();
        return;
    }

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
            
            // Iterate through all the Sampler2Ds that the shader program requests
            const std::vector<ShaderProgramResource::Control>& sampler2DControls = mShaderProg->getSampler2Ds();
            for(std::vector<ShaderProgramResource::Control>::const_iterator iter = sampler2DControls.begin(); iter != sampler2DControls.end(); ++ iter) {
                const ShaderProgramResource::Control& entry = *iter;
                
                Sampler2DInput input;
                input.handle = entry.handle;

                // Find the appropriate texture
                const Json::Value& value = sampler2Ds[entry.name];
                if(!value.isNull()) {
                    input.texture = rmgr->findTexture(value.asString());
                } else { // Texture receives no input
                    input.texture = rmgr->getFallbackTexture();
                }
                
                input.texture->grab();
                mSampler2Ds.push_back(input);
            }

        }
        else {
            // replace with error texture
        }
    }

    mLoaded = true;
}

void MaterialResource::unload() {
    assert(mLoaded && "Attempted to unload material before loading it");

    if(mIsErrorResource) {
        unloadError();
        return;
    }

    mShaderProg->drop();

    for(std::vector<Sampler2DInput>::iterator iter = mSampler2Ds.begin(); iter != mSampler2Ds.end(); ++ iter) {
        Sampler2DInput& input = *iter;

        input.texture->drop();
    }

    mLoaded = false;
}

void MaterialResource::bindTextures() {
    unsigned int index = 0;
    for(std::vector<Sampler2DInput>::iterator iter = mSampler2Ds.begin(); iter != mSampler2Ds.end(); ++ iter) {
        Sampler2DInput& control = *iter;
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
