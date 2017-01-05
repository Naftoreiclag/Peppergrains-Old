/*
   Copyright 2015-2017 James Fong

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
#include "TextureResource.hpp"

namespace pgg {

MaterialResource::MaterialResource()
: mLoaded(false)
, Resource(Resource::Type::MATERIAL) {
}

MaterialResource::~MaterialResource() {
}

Material* MaterialResource::gallop(Resource* resource) {
    if(!resource || resource->mResourceType != Resource::Type::MATERIAL) {
        Logger::log(Logger::WARN) << "Failed to cast " << (resource ? resource->getName() : "nullptr") << " to material!" << std::endl;
        return Material::getFallback();
    } else {
        return static_cast<MaterialResource*>(resource);
    }
}

Material::Input MaterialResource::jsonToMaterialInput(const Json::Value& inputData) {
    if(inputData.isObject()) {
        const Json::Value& typeData = inputData["type"];
        
        if(!typeData.isNull()) {
            
            if(typeData.asString() == "texture") {
                return Material::Input(TextureResource::gallop(Resources::find(inputData["value"].asString())));
            } else if(typeData.asString() == "vec3") {
                return Material::Input();
            }
        }
    }
    return Material::Input();
}

void MaterialResource::load() {
    assert(!mLoaded && "Attempted to load material that has already been loaded");
    // Fallback is a special resource
    if(this->isFallback()) {
        mTechnique.mType = Material::Technique::Type::HIGH_LEVEL_VALUES;
        mTechnique.mDiffuse = Material::Input(Texture::getFallback());
        
        mLoaded = true;
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
                mTechnique.mType = Material::Technique::Type::GLSL_SHADER;
                // TODO: implement me
                
                if(false) {
                    break;
                }
            }
            else if(typeData.asString() == "high-level-values") {
                mTechnique.mType = Material::Technique::Type::HIGH_LEVEL_VALUES;
                
                mTechnique.mDiffuse = jsonToMaterialInput(techniqueData["diffuse"]);
                mTechnique.mSpecular = jsonToMaterialInput(techniqueData["specular"]);
                mTechnique.mNormals = jsonToMaterialInput(techniqueData["normals"]);
                
                break;
            }
            
        }
    
    }
    mLoaded = true;
}

void MaterialResource::unload() {
    assert(mLoaded && "Attempted to unload material before loading it");
    mTechnique.clear();
    mLoaded = false;
}

/*
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
        if(mTechnique.normals->isSpecified()) {
            mTechnique.deferredGeometryProg = ShaderProgramResource::gallop(Resources::find("HLVSDiffuseTexNormalTex.shaderProgram"));
            mTechnique.deferredGeometryProg->grab();
        } else {
            mTechnique.deferredGeometryProg = ShaderProgramResource::gallop(Resources::find("HLVSDiffuseTex.shaderProgram"));
            mTechnique.deferredGeometryProg->grab();
        }
        
        mTechnique.shoForwardProg = ShaderProgramResource::gallop(Resources::find("HLVSShoForwardDiffuseTex.shaderProgram"));
        mTechnique.shoForwardProg->grab();
        
        if(mTechnique.ssipgSpots->isSpecified()) {
            mTechnique.ssipgPassProg = ShaderProgramResource::gallop(Resources::find("SSIPG.shaderProgram"));
            mTechnique.ssipgPassProg->grab();
        }
    }
}
*/

/*

void MaterialResource::enableVertexAttributesFor(Geometry* geometry) const {
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
void MaterialResource::useProgram(Renderable::Pass rendPass, const glm::mat4& modelMatrix) const {
    
    if(rendPass.mType == Renderable::Pass::Type::SHO_FORWARD) {
        
        // Tell OpenGL to use that shader program
        glUseProgram(mTechnique.shoForwardProg->getHandle());

        // Tell OpenGL to use the provided matrices
        mTechnique.shoForwardProg->bindRenderPass(rendPass, modelMatrix);
        
        // Bind the textures specified by the material
        unsigned int index = 0;
        
        // TODO pre-calculate this somewhere
        for(const ShaderProgramResource::Control& control : mTechnique.shoForwardProg->getUniformSampler2Ds()) {
            if(control.name == "diffuse" && mTechnique.diffuse->isSpecified()) {
                glActiveTexture(GL_TEXTURE0 + index);
                glBindTexture(GL_TEXTURE_2D, mTechnique.diffuse->textureValue->getHandle());
                glUniform1i(control.handle, index);
                ++ index;
            }
            if(control.name == "normals" && mTechnique.normals->isSpecified()) {
                glActiveTexture(GL_TEXTURE0 + index);
                glBindTexture(GL_TEXTURE_2D, mTechnique.normals->textureValue->getHandle());
                glUniform1i(control.handle, index);
                ++ index;
            }
        }
        
        
        
    }
    else if(rendPass.mType == Renderable::Pass::Type::GEOMETRY || rendPass.mType == Renderable::Pass::Type::SHADOW) {
        // Tell OpenGL to use that shader program
        glUseProgram(mTechnique.deferredGeometryProg->getHandle());

        // Tell OpenGL to use the provided matrices
        mTechnique.deferredGeometryProg->bindRenderPass(rendPass, modelMatrix);

        // Bind the textures specified by the material
        unsigned int index = 0;
        
        // TODO pre-calculate this somewhere
        for(const ShaderProgramResource::Control& control : mTechnique.deferredGeometryProg->getUniformSampler2Ds()) {
            if(control.name == "diffuse" && mTechnique.diffuse->isSpecified()) {
                glActiveTexture(GL_TEXTURE0 + index);
                glBindTexture(GL_TEXTURE_2D, mTechnique.diffuse->textureValue->getHandle());
                glUniform1i(control.handle, index);
                ++ index;
            }
            if(control.name == "normals" && mTechnique.normals->isSpecified()) {
                glActiveTexture(GL_TEXTURE0 + index);
                glBindTexture(GL_TEXTURE_2D, mTechnique.normals->textureValue->getHandle());
                glUniform1i(control.handle, index);
                ++ index;
            }
        }
    }
    else if(rendPass.mType == Renderable::Pass::Type::SSIPG && mTechnique.ssipgPassProg != nullptr) {
        glUseProgram(mTechnique.ssipgPassProg->getHandle());
        
        mTechnique.ssipgPassProg->bindRenderPass(rendPass, modelMatrix);
        
        unsigned int index = 0;
        for(std::vector<ShaderProgramResource::Control>::const_iterator iter = mTechnique.ssipgPassProg->getUniformSampler2Ds().begin(); iter != mTechnique.ssipgPassProg->getUniformSampler2Ds().end(); ++ iter) {
            const ShaderProgramResource::Control& control = *iter;
            
            if(control.name == "spots" && mTechnique.ssipgSpots->isSpecified()) {
                glActiveTexture(GL_TEXTURE0 + index);
                glBindTexture(GL_TEXTURE_2D, mTechnique.ssipgSpots->textureValue->getHandle());
                glUniform1i(control.handle, index);
                ++ index;
            }
        }
    }
}
bool MaterialResource::isVisible(Renderable::Pass rpc) const {
    if(rpc.mType == Renderable::Pass::Type::GEOMETRY) {
        return mTechnique.deferredGeometryProg != nullptr;
    }
    
    if(rpc.mType == Renderable::Pass::Type::SHADOW) {
        return mTechnique.deferredGeometryProg != nullptr;
    }
    
    if(rpc.mType == Renderable::Pass::Type::SSIPG) {
        return mTechnique.ssipgPassProg != nullptr;
    }
    
    if(rpc.mType == Renderable::Pass::Type::SHO_FORWARD) {
        return true;
    }
}

const ShaderProgramResource* MaterialResource::getShaderProg() const {
    return mTechnique.deferredGeometryProg;
}
*/

}
