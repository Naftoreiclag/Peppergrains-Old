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



MaterialResource::MaterialInput::MaterialInput(const Json::Value& inputData) {
    // Until proven otherwise
    type = Type::NOTHING;
    
    if(inputData.isObject()) {
        const Json::Value& typeData = inputData["type"];
        
        if(!typeData.isNull()) {
            
            ResourceManager* resman = ResourceManager::getSingleton();
            
            if(typeData.asString() == "texture") {
                type = Type::TEXTURE;
                
                textureValue = resman->findTexture(inputData["value"].asString());
                textureValue->grab();
            } else if(typeData.asString() == "constant") {
                
            }
        }
    }
}
MaterialResource::MaterialInput::~MaterialInput() {
    switch(type) {
        case Type::TEXTURE: {
            textureValue->drop();
            break;
        }
        default: break;
    }
}
bool MaterialResource::MaterialInput::isNothing() const { return type == Type::NOTHING; }

MaterialResource::Technique::Technique()
: type(Type::NONE) {
}
MaterialResource::Technique::~Technique() {
    
}
MaterialResource::Technique::Type MaterialResource::getTechniqueType() const {
    return mTechnique.type;
}

MaterialResource::MaterialResource()
: mLoaded(false)
, mIsErrorResource(false) {
}

MaterialResource::~MaterialResource() {
}

void MaterialResource::loadError() {
    assert(!mLoaded && "Attempted to load material that has already been loaded");
    
    ResourceManager* rmgr = ResourceManager::getSingleton();
    
    /*
    mShaderProg = rmgr->getFallbackShaderProgram();
    mShaderProg->grab();
    
    // Iterate through all the Sampler2Ds that the shader program requests
    const std::vector<ShaderProgramResource::Control>& sampler2DControls = mShaderProg->getUniformSampler2Ds();
    for(std::vector<ShaderProgramResource::Control>::const_iterator iter = sampler2DControls.begin(); iter != sampler2DControls.end(); ++ iter) {
        const ShaderProgramResource::Control& entry = *iter;
        
        Sampler2DInput input;
        input.handle = entry.handle;
        input.texture = rmgr->getFallbackTexture();
        input.texture->grab();
        mSampler2Ds.push_back(input);
    }
    */
    
    mLoaded = true;
    mIsErrorResource = true;
    
    std::cout << "Material error: " << this->getName() << std::endl;
}

void MaterialResource::unloadError() {
    assert(mLoaded && "Attempted to unload material before loading it");
    
    /*
    mShaderProg->drop();

    for(std::vector<Sampler2DInput>::iterator iter = mSampler2Ds.begin(); iter != mSampler2Ds.end(); ++ iter) {
        Sampler2DInput& input = *iter;

        input.texture->drop();
    }
     */
    
    mLoaded = false;
    mIsErrorResource = false;
}

void MaterialResource::load() {
    assert(!mLoaded && "Attempted to load material that has already been loaded");

    if(this->isFallback()) {
        loadError();
        return;
    }

    ResourceManager* resman = ResourceManager::getSingleton();

    Json::Value matData;
    {
        std::ifstream loader(this->getFile().string().c_str());
        loader >> matData;
        loader.close();
    }
    
    const Json::Value& techniqueListData = matData["techniques"];
    
    mTechnique.type = Technique::Type::NONE;
    
    
    std::cout << "b";
    if(techniqueListData.isArray()) {
    std::cout << "e";
        for(Json::Value::const_iterator iter = techniqueListData.begin(); iter != techniqueListData.end(); ++ iter) {
    std::cout << "a";
            const Json::Value& techniqueData = *iter;
            
            
    std::cout << "a";
            const Json::Value& typeData = techniqueData["type"];
            
    std::cout << "a";
            if(typeData.asString() == "glsl-shader") {
                mTechnique.type = Technique::Type::GLSL_SHADER;
                // ???
                
                if(false) {
                    break;
                }
            }
            else if(typeData.asString() == "high-level-values") {
                mTechnique.type = Technique::Type::HIGH_LEVEL_VALUES;
                
    std::cout << "a";
                mTechnique.diffuse = new MaterialInput(techniqueData["diffuse"]);
                mTechnique.specular = new MaterialInput(techniqueData["specular"]);
                mTechnique.normals = new MaterialInput(techniqueData["normals"]);
                mTechnique.ssipgMap = new MaterialInput(techniqueData["ssipg-map"]);
                mTechnique.ssipgFlow = new MaterialInput(techniqueData["ssipg-flow"]);
                
    std::cout << "a";
                if(!mTechnique.normals->isNothing()) {
                    mTechnique.shaderProg = resman->findShaderProgram("HLVSDiffuseTexNormalTex.shaderProgram");
                } else {
                    mTechnique.shaderProg = resman->findShaderProgram("HLVSDiffuseTex.shaderProgram");
                }
                mTechnique.shaderProg->grab();
                
                
    std::cout << "a";
                
                break;
            }
            
        }
    std::cout << "a";
    
    }
    std::cout << "c";

    /*
    mShaderProg = rmgr->findShaderProgram(matData["shaderProgram"].asString());
    mShaderProg->grab();

    // Find all Sampler2Ds
    {
        const Json::Value& sampler2Ds = matData["sampler2D"];

        if(!sampler2Ds.isNull()) {
            
            // Iterate through all the Sampler2Ds that the shader program requests
            const std::vector<ShaderProgramResource::Control>& sampler2DControls = mShaderProg->getUniformSampler2Ds();
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
    */

    mLoaded = true;
}

void MaterialResource::unload() {
    assert(mLoaded && "Attempted to unload material before loading it");

    switch(mTechnique.type) {
        case Technique::Type::HIGH_LEVEL_VALUES: {
            delete mTechnique.diffuse;
            delete mTechnique.specular;
            delete mTechnique.normals;
            delete mTechnique.ssipgMap;
            delete mTechnique.ssipgFlow;
            break;
        }
        default: break;
    }
    mTechnique.type = Technique::Type::NONE;

    /*
    if(mIsErrorResource) {
        unloadError();
        return;
    }

    mShaderProg->drop();

    for(std::vector<Sampler2DInput>::iterator iter = mSampler2Ds.begin(); iter != mSampler2Ds.end(); ++ iter) {
        Sampler2DInput& input = *iter;

        input.texture->drop();
    }
    */

    mLoaded = false;
}

void MaterialResource::enableVertexAttributesFor(GeometryResource* geometry) const {
    if(mTechnique.shaderProg->needsPosAttrib()) {
        geometry->enablePositionAttrib(mTechnique.shaderProg->getPosAttrib());
    }
    if(mTechnique.shaderProg->needsColorAttrib()) {
        geometry->enableColorAttrib(mTechnique.shaderProg->getColorAttrib());
    }
    if(mTechnique.shaderProg->needsUVAttrib()) {
        geometry->enableUVAttrib(mTechnique.shaderProg->getUVAttrib());
    }
    if(mTechnique.shaderProg->needsNormalAttrib()) {
        geometry->enableNormalAttrib(mTechnique.shaderProg->getNormalAttrib());
    }
    if(mTechnique.shaderProg->needsTangentAttrib()) {
        geometry->enableTangentAttrib(mTechnique.shaderProg->getTangentAttrib());
    }
    if(mTechnique.shaderProg->needsBitangentAttrib()) {
        geometry->enableBitangentAttrib(mTechnique.shaderProg->getBitangentAttrib());
    }
}
void MaterialResource::use(const glm::mat4& mMat, const glm::mat4& vMat, const glm::mat4& pMat) const {
    
    // Tell OpenGL to use that shader program
    glUseProgram(mTechnique.shaderProg->getHandle());

    // Tell OpenGL to use the provided matrices
    mTechnique.shaderProg->bindModelViewProjMatrices(mMat, vMat, pMat);

    // Bind the textures specified by the material
    unsigned int index = 0;
    for(std::vector<ShaderProgramResource::Control>::const_iterator iter = mTechnique.shaderProg->getUniformSampler2Ds().begin(); iter != mTechnique.shaderProg->getUniformSampler2Ds().end(); ++ iter) {
        const ShaderProgramResource::Control& control = *iter;
        
        if(control.name == "diffuse" && !mTechnique.diffuse->isNothing()) {
            glActiveTexture(GL_TEXTURE0 + index);
            glBindTexture(GL_TEXTURE_2D, mTechnique.diffuse->textureValue->getHandle());
            glUniform1i(control.handle, index);
            ++ index;
        }
        if(control.name == "normals" && !mTechnique.normals->isNothing()) {
            glActiveTexture(GL_TEXTURE0 + index);
            glBindTexture(GL_TEXTURE_2D, mTechnique.normals->textureValue->getHandle());
            glUniform1i(control.handle, index);
            ++ index;
        }
    }
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
    return mTechnique.shaderProg;
}

}
