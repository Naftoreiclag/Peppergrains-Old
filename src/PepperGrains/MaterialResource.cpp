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
MaterialResource::MaterialInput::MaterialInput(TextureResource* textureRes) {
    type = Type::TEXTURE;
    textureValue = textureRes;
    textureValue->grab();
}

MaterialResource::MaterialInput::MaterialInput() {
    type = Type::NOTHING;
}

MaterialResource::MaterialInput::~MaterialInput() {
    switch(type) {
        case Type::TEXTURE: {
            textureValue->drop();
            textureValue = nullptr;
            break;
        }
        default: break;
    }
}
bool MaterialResource::MaterialInput::specified() const { return type != Type::NOTHING; }

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
    mTechnique.shaderProg = nullptr;
}

MaterialResource::~MaterialResource() {
}

void MaterialResource::grabNeededHLVShader() {
    ResourceManager* resman = ResourceManager::getSingleton();
    if(mTechnique.shaderProg) {
        mTechnique.shaderProg->drop();
        mTechnique.shaderProg = nullptr;
    }
    if(mTechnique.normals->specified()) {
        mTechnique.shaderProg = resman->findShaderProgram("HLVSDiffuseTexNormalTex.shaderProgram");
    } else {
        mTechnique.shaderProg = resman->findShaderProgram("HLVSDiffuseTex.shaderProgram");
    }
    mTechnique.shaderProg->grab();
}

void MaterialResource::loadError() {
    assert(!mLoaded && "Attempted to load material that has already been loaded");
    
    ResourceManager* resman = ResourceManager::getSingleton();
    
    mTechnique.type = Technique::Type::HIGH_LEVEL_VALUES;
    mTechnique.diffuse = new MaterialInput(resman->getFallbackTexture());
    mTechnique.specular = new MaterialInput();
    mTechnique.normals = new MaterialInput();
    mTechnique.ssipgMap = new MaterialInput();
    mTechnique.ssipgFlow = new MaterialInput();
    grabNeededHLVShader();
    
    mLoaded = true;
    mIsErrorResource = true;
    
    std::cout << "Material error: " << this->getName() << std::endl;
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
    
    if(techniqueListData.isArray()) {
        for(Json::Value::const_iterator iter = techniqueListData.begin(); iter != techniqueListData.end(); ++ iter) {
            const Json::Value& techniqueData = *iter;
            
            
            const Json::Value& typeData = techniqueData["type"];
            
            if(typeData.asString() == "glsl-shader") {
                mTechnique.type = Technique::Type::GLSL_SHADER;
                // ???
                
                if(false) {
                    break;
                }
            }
            else if(typeData.asString() == "high-level-values") {
                mTechnique.type = Technique::Type::HIGH_LEVEL_VALUES;
                
                mTechnique.diffuse = new MaterialInput(techniqueData["diffuse"]);
                mTechnique.specular = new MaterialInput(techniqueData["specular"]);
                mTechnique.normals = new MaterialInput(techniqueData["normals"]);
                mTechnique.ssipgMap = new MaterialInput(techniqueData["ssipg-map"]);
                mTechnique.ssipgFlow = new MaterialInput(techniqueData["ssipg-flow"]);
                grabNeededHLVShader();
                
                break;
            }
            
        }
    
    }
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
            
            mTechnique.shaderProg->drop();
            mTechnique.shaderProg = nullptr;
            
            break;
        }
        default: break;
    }
    mTechnique.type = Technique::Type::NONE;

    mLoaded = false;
    mIsErrorResource = false;
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
void MaterialResource::use(const Model::RenderPass& rpc, const glm::mat4& mMat) const {
    
    // Tell OpenGL to use that shader program
    glUseProgram(mTechnique.shaderProg->getHandle());

    // Tell OpenGL to use the provided matrices
    mTechnique.shaderProg->bindModelViewProjMatrices(mMat, rpc.viewMat, rpc.projMat);

    // Bind the textures specified by the material
    unsigned int index = 0;
    for(std::vector<ShaderProgramResource::Control>::const_iterator iter = mTechnique.shaderProg->getUniformSampler2Ds().begin(); iter != mTechnique.shaderProg->getUniformSampler2Ds().end(); ++ iter) {
        const ShaderProgramResource::Control& control = *iter;
        
        if(control.name == "diffuse" && mTechnique.diffuse->specified()) {
            glActiveTexture(GL_TEXTURE0 + index);
            glBindTexture(GL_TEXTURE_2D, mTechnique.diffuse->textureValue->getHandle());
            glUniform1i(control.handle, index);
            ++ index;
        }
        if(control.name == "normals" && mTechnique.normals->specified()) {
            glActiveTexture(GL_TEXTURE0 + index);
            glBindTexture(GL_TEXTURE_2D, mTechnique.normals->textureValue->getHandle());
            glUniform1i(control.handle, index);
            ++ index;
        }
    }
}
bool MaterialResource::isVisible(const Model::RenderPass& rpc) const {
    if(rpc.type == Model::RenderPass::Type::GEOMETRY) {
        return true;
    }
    
    if(rpc.type == Model::RenderPass::Type::SHADOW) {
        return true;
    }
    
    if(rpc.type == Model::RenderPass::Type::SSIPG) {
        return mTechnique.ssipgMap->specified();
    }
}

const ShaderProgramResource* MaterialResource::getShaderProg() const {
    return mTechnique.shaderProg;
}

}
