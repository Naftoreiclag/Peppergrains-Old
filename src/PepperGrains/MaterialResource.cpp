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
, mIsErrorResource(false)
, Resource(Resource::Type::MATERIAL) {
    mTechnique.geometryPassProg = nullptr;
    mTechnique.ssipgPassProg = nullptr;
}

MaterialResource::~MaterialResource() {
}

void MaterialResource::grabNeededHLVShaders() {
    ResourceManager* resman = ResourceManager::getSingleton();
    if(mTechnique.geometryPassProg) {
        mTechnique.geometryPassProg->drop();
        mTechnique.geometryPassProg = nullptr;
    }
    if(mTechnique.ssipgPassProg) {
        mTechnique.ssipgPassProg->drop();
        mTechnique.ssipgPassProg = nullptr;
    }
    
    if(mTechnique.type == Technique::Type::HIGH_LEVEL_VALUES) {
        if(mTechnique.normals->specified()) {
            mTechnique.geometryPassProg = resman->findShaderProgram("HLVSDiffuseTexNormalTex.shaderProgram");
            mTechnique.geometryPassProg->grab();
        } else {
            mTechnique.geometryPassProg = resman->findShaderProgram("HLVSDiffuseTex.shaderProgram");
            mTechnique.geometryPassProg->grab();
        }
        if(mTechnique.ssipgSpots->specified()) {
            mTechnique.ssipgPassProg = resman->findShaderProgram("SSIPG.shaderProgram");
            mTechnique.ssipgPassProg->grab();
        }
    }
}

void MaterialResource::loadError() {
    assert(!mLoaded && "Attempted to load material that has already been loaded");
    
    ResourceManager* resman = ResourceManager::getSingleton();
    
    mTechnique.type = Technique::Type::HIGH_LEVEL_VALUES;
    mTechnique.diffuse = new MaterialInput(resman->getFallbackTexture());
    mTechnique.specular = new MaterialInput();
    mTechnique.normals = new MaterialInput();
    mTechnique.ssipgSpots = new MaterialInput();
    mTechnique.ssipgFlow = new MaterialInput();
    grabNeededHLVShaders();
    
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
                mTechnique.ssipgSpots = new MaterialInput(techniqueData["ssipg-spots"]);
                mTechnique.ssipgFlow = new MaterialInput(techniqueData["ssipg-flow"]);
                grabNeededHLVShaders();
                
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
            delete mTechnique.ssipgSpots;
            delete mTechnique.ssipgFlow;
            
            mTechnique.geometryPassProg->drop();
            mTechnique.geometryPassProg = nullptr;
            
            break;
        }
        default: break;
    }
    mTechnique.type = Technique::Type::NONE;

    mLoaded = false;
    mIsErrorResource = false;
}

void MaterialResource::enableVertexAttributesFor(GeometryResource* geometry) const {
    if(mTechnique.geometryPassProg->needsPosAttrib()) {
        geometry->enablePositionAttrib(mTechnique.geometryPassProg->getPosAttrib());
    }
    if(mTechnique.geometryPassProg->needsColorAttrib()) {
        geometry->enableColorAttrib(mTechnique.geometryPassProg->getColorAttrib());
    }
    if(mTechnique.geometryPassProg->needsUVAttrib()) {
        geometry->enableUVAttrib(mTechnique.geometryPassProg->getUVAttrib());
    }
    if(mTechnique.geometryPassProg->needsNormalAttrib()) {
        geometry->enableNormalAttrib(mTechnique.geometryPassProg->getNormalAttrib());
    }
    if(mTechnique.geometryPassProg->needsTangentAttrib()) {
        geometry->enableTangentAttrib(mTechnique.geometryPassProg->getTangentAttrib());
    }
    if(mTechnique.geometryPassProg->needsBitangentAttrib()) {
        geometry->enableBitangentAttrib(mTechnique.geometryPassProg->getBitangentAttrib());
    }
}
void MaterialResource::use(Renderable::Pass rpc, const glm::mat4& mMat) const {
    
    if(rpc.type == Renderable::Pass::Type::GEOMETRY || rpc.type == Renderable::Pass::Type::SHADOW) {
        // Tell OpenGL to use that shader program
        glUseProgram(mTechnique.geometryPassProg->getHandle());

        // Tell OpenGL to use the provided matrices
        mTechnique.geometryPassProg->bindRenderPass(rpc, mMat);

        // Bind the textures specified by the material
        unsigned int index = 0;
        
        // TODO pre-calculate this somewhere
        for(std::vector<ShaderProgramResource::Control>::const_iterator iter = mTechnique.geometryPassProg->getUniformSampler2Ds().begin(); iter != mTechnique.geometryPassProg->getUniformSampler2Ds().end(); ++ iter) {
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
    else if(rpc.type == Renderable::Pass::Type::SSIPG && mTechnique.ssipgPassProg != nullptr) {
        glUseProgram(mTechnique.ssipgPassProg->getHandle());
        
        mTechnique.ssipgPassProg->bindRenderPass(rpc, mMat);
        
        unsigned int index = 0;
        for(std::vector<ShaderProgramResource::Control>::const_iterator iter = mTechnique.ssipgPassProg->getUniformSampler2Ds().begin(); iter != mTechnique.ssipgPassProg->getUniformSampler2Ds().end(); ++ iter) {
            const ShaderProgramResource::Control& control = *iter;
            
            if(control.name == "spots" && mTechnique.ssipgSpots->specified()) {
                glActiveTexture(GL_TEXTURE0 + index);
                glBindTexture(GL_TEXTURE_2D, mTechnique.ssipgSpots->textureValue->getHandle());
                glUniform1i(control.handle, index);
                ++ index;
            }
        }
    }
}
bool MaterialResource::isVisible(Renderable::Pass rpc) const {
    if(rpc.type == Renderable::Pass::Type::GEOMETRY) {
        return mTechnique.geometryPassProg != nullptr;
    }
    
    if(rpc.type == Renderable::Pass::Type::SHADOW) {
        return mTechnique.geometryPassProg != nullptr;
    }
    
    if(rpc.type == Renderable::Pass::Type::SSIPG) {
        return mTechnique.ssipgPassProg != nullptr;
    }
}

const ShaderProgramResource* MaterialResource::getShaderProg() const {
    return mTechnique.geometryPassProg;
}

}
