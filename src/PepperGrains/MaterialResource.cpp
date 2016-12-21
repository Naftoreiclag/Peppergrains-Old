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

#include "ShaderProgramResource.hpp"
#include "Logger.hpp"
#include "Resources.hpp"

namespace pgg {

MaterialResource::MaterialInput::MaterialInput(const Json::Value& inputData) {
    // Until proven otherwise
    type = Type::NOTHING;
    
    if(inputData.isObject()) {
        const Json::Value& typeData = inputData["type"];
        
        if(!typeData.isNull()) {
            
            if(typeData.asString() == "texture") {
                type = Type::TEXTURE;
                
                textureValue = TextureResource::upcast(Resources::find(inputData["value"].asString()));
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
    mTechnique.deferredGeometryProg = nullptr;
    mTechnique.ssipgPassProg = nullptr;
}

MaterialResource::~MaterialResource() {
}

MaterialResource* MaterialResource::upcast(Resource* resource) {
    if(!resource || resource->mResourceType != Resource::Type::MATERIAL) {
        Logger::log(Logger::WARN) << "Failed to cast " << resource->getName() << " to material!" << std::endl;
        return getFallback();
    } else {
        return static_cast<MaterialResource*>(resource);
    }
}

MaterialResource* MaterialResource::getFallback() {
    return nullptr;
}

void MaterialResource::grabNeededHLVShaders() {
    if(mTechnique.deferredGeometryProg) {
        mTechnique.deferredGeometryProg->drop();
        mTechnique.deferredGeometryProg = nullptr;
    }
    if(mTechnique.ssipgPassProg) {
        mTechnique.ssipgPassProg->drop();
        mTechnique.ssipgPassProg = nullptr;
    }
    
    if(mTechnique.type == Technique::Type::HIGH_LEVEL_VALUES) {
        if(mTechnique.normals->specified()) {
            mTechnique.deferredGeometryProg = ShaderProgramResource::upcast(Resources::find("HLVSDiffuseTexNormalTex.shaderProgram"));
            mTechnique.deferredGeometryProg->grab();
        } else {
            mTechnique.deferredGeometryProg = ShaderProgramResource::upcast(Resources::find("HLVSDiffuseTex.shaderProgram"));
            mTechnique.deferredGeometryProg->grab();
        }
        
        mTechnique.shoForwardProg = ShaderProgramResource::upcast(Resources::find("HLVSShoForwardDiffuseTex.shaderProgram"));
        mTechnique.shoForwardProg->grab();
        
        if(mTechnique.ssipgSpots->specified()) {
            mTechnique.ssipgPassProg = ShaderProgramResource::upcast(Resources::find("SSIPG.shaderProgram"));
            mTechnique.ssipgPassProg->grab();
        }
    }
}

void MaterialResource::loadError() {
    assert(!mLoaded && "Attempted to load material that has already been loaded");
    
    mTechnique.type = Technique::Type::HIGH_LEVEL_VALUES;
    mTechnique.diffuse = new MaterialInput(TextureResource::getFallback());
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
                // TODO: implement me
                
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
            
            mTechnique.deferredGeometryProg->drop();
            mTechnique.deferredGeometryProg = nullptr;
            if(mTechnique.ssipgPassProg) {
                mTechnique.ssipgPassProg->drop();
                mTechnique.ssipgPassProg = nullptr;
            }
            mTechnique.shoForwardProg->drop();
            mTechnique.shoForwardProg = nullptr;
            
            break;
        }
        default: break;
    }
    mTechnique.type = Technique::Type::NONE;

    mLoaded = false;
    mIsErrorResource = false;
}

void MaterialResource::enableVertexAttributesFor(GeometryResource* geometry) const {
    if(mTechnique.deferredGeometryProg->needsPosAttrib()) {
        geometry->enablePositionAttrib(mTechnique.deferredGeometryProg->getPosAttrib());
    }
    if(mTechnique.deferredGeometryProg->needsColorAttrib()) {
        geometry->enableColorAttrib(mTechnique.deferredGeometryProg->getColorAttrib());
    }
    if(mTechnique.deferredGeometryProg->needsUVAttrib()) {
        geometry->enableUVAttrib(mTechnique.deferredGeometryProg->getUVAttrib());
    }
    if(mTechnique.deferredGeometryProg->needsNormalAttrib()) {
        geometry->enableNormalAttrib(mTechnique.deferredGeometryProg->getNormalAttrib());
    }
    if(mTechnique.deferredGeometryProg->needsTangentAttrib()) {
        geometry->enableTangentAttrib(mTechnique.deferredGeometryProg->getTangentAttrib());
    }
    if(mTechnique.deferredGeometryProg->needsBitangentAttrib()) {
        geometry->enableBitangentAttrib(mTechnique.deferredGeometryProg->getBitangentAttrib());
    }
}
void MaterialResource::use(Renderable::Pass rendPass, const glm::mat4& modelMatrix) const {
    
    if(rendPass.type == Renderable::Pass::Type::SHO_FORWARD) {
        // Tell OpenGL to use that shader program
        glUseProgram(mTechnique.shoForwardProg->getHandle());

        // Tell OpenGL to use the provided matrices
        mTechnique.shoForwardProg->bindRenderPass(rendPass, modelMatrix);
        
        // Bind the textures specified by the material
        unsigned int index = 0;
        
        // TODO pre-calculate this somewhere
        for(const ShaderProgramResource::Control& control : mTechnique.shoForwardProg->getUniformSampler2Ds()) {
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
    else if(rendPass.type == Renderable::Pass::Type::GEOMETRY || rendPass.type == Renderable::Pass::Type::SHADOW) {
        // Tell OpenGL to use that shader program
        glUseProgram(mTechnique.deferredGeometryProg->getHandle());

        // Tell OpenGL to use the provided matrices
        mTechnique.deferredGeometryProg->bindRenderPass(rendPass, modelMatrix);

        // Bind the textures specified by the material
        unsigned int index = 0;
        
        // TODO pre-calculate this somewhere
        for(const ShaderProgramResource::Control& control : mTechnique.deferredGeometryProg->getUniformSampler2Ds()) {
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
    else if(rendPass.type == Renderable::Pass::Type::SSIPG && mTechnique.ssipgPassProg != nullptr) {
        glUseProgram(mTechnique.ssipgPassProg->getHandle());
        
        mTechnique.ssipgPassProg->bindRenderPass(rendPass, modelMatrix);
        
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
        return mTechnique.deferredGeometryProg != nullptr;
    }
    
    if(rpc.type == Renderable::Pass::Type::SHADOW) {
        return mTechnique.deferredGeometryProg != nullptr;
    }
    
    if(rpc.type == Renderable::Pass::Type::SSIPG) {
        return mTechnique.ssipgPassProg != nullptr;
    }
}

const ShaderProgramResource* MaterialResource::getShaderProg() const {
    return mTechnique.deferredGeometryProg;
}

}
