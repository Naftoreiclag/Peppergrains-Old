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

#include "ResourceManager.hpp"

#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <map>

#include "json/json.h"

namespace pgg {

ResourceManager* ResourceManager::getSingleton() {
    static ResourceManager instance;

    return &instance;
}

ResourceManager::ResourceManager()
: mFallbacksGrabbed(false)
, mAddonsLoaded(false) {
    mFallbackString = new StringResource();
    mFallbackImage = new ImageResource();
    mFallbackTexture = new TextureResource();
    mFallbackModel = new ModelResource();
    mFallbackMaterial = new MaterialResource();
    mFallbackGeometry = new GeometryResource();
    mFallbackShader = new ShaderResource(ShaderResource::Type::FRAGMENT);
    mFallbackShaderProgram = new ShaderProgramResource();
    mFallbackFont = new FontResource();
}

ResourceManager::~ResourceManager() {
    if(mFallbacksGrabbed) {
        mFallbackString->drop();
        mFallbackImage->drop();
        mFallbackTexture->drop();
        mFallbackModel->drop();
        mFallbackMaterial->drop();
        mFallbackGeometry->drop();
        mFallbackShader->drop();
        mFallbackShaderProgram->drop();
        mFallbackFont->drop();
    }
    
    delete mFallbackString;
    delete mFallbackImage;
    delete mFallbackTexture;
    delete mFallbackModel;
    delete mFallbackMaterial;
    delete mFallbackGeometry;
    delete mFallbackShader;
    delete mFallbackShaderProgram;
    delete mFallbackFont;
}

StringResource* ResourceManager::getFallbackString() { return mFallbackString; }
ImageResource* ResourceManager::getFallbackImage() { return mFallbackImage; }
TextureResource* ResourceManager::getFallbackTexture() { return mFallbackTexture; }
ModelResource* ResourceManager::getFallbackModel() { return mFallbackModel; }
MaterialResource* ResourceManager::getFallbackMaterial() { return mFallbackMaterial; }
GeometryResource* ResourceManager::getFallbackGeometry() { return mFallbackGeometry; }
ShaderResource* ResourceManager::getFallbackShader() { return mFallbackShader; }
ShaderProgramResource* ResourceManager::getFallbackShaderProgram() { return mFallbackShaderProgram; }
FontResource* ResourceManager::getFallbackFont() { return mFallbackFont; }

void ResourceManager::mapAll(boost::filesystem::path dataPackFile) {
    Json::Value dataPackData;
    
    {
        std::ifstream reader(dataPackFile.string().c_str());
        reader >> dataPackData;
        reader.close();
    }
    
    boost::filesystem::path dataPackDir = dataPackFile.parent_path();
    
    const Json::Value& resourcesData = dataPackData["resources"];
    
    for(Json::Value::const_iterator iter = resourcesData.begin(); iter != resourcesData.end(); ++ iter) {
        const Json::Value& resourceData = *iter;
        
        std::string resType = resourceData["type"].asString();
        std::string name = resourceData["name"].asString();
        std::string file = resourceData["file"].asString();
        uint32_t size = resourceData["size"].asInt();
        
        Resource* newRes;
        if(resType == "text") {
            newRes = mStrings[name] = new StringResource();
        } else if(resType == "compute-shader") {
            newRes = mShaders[name] = new ShaderResource(ShaderResource::Type::COMPUTE);
        } else if(resType == "vertex-shader") {
            newRes = mShaders[name] = new ShaderResource(ShaderResource::Type::VERTEX);
        } else if(resType == "tess-control-shader") {
            newRes = mShaders[name] = new ShaderResource(ShaderResource::Type::TESS_CONTROL);
        } else if(resType == "tess-evaluation-shader") {
            newRes = mShaders[name] = new ShaderResource(ShaderResource::Type::TESS_EVALUATION);
        } else if(resType == "geometry-shader") {
            newRes = mShaders[name] = new ShaderResource(ShaderResource::Type::GEOMETRY);
        } else if(resType == "fragment-shader") {
            newRes = mShaders[name] = new ShaderResource(ShaderResource::Type::FRAGMENT);
        } else if(resType == "shader-program") {
            newRes = mShaderPrograms[name] = new ShaderProgramResource();
        } else if(resType == "image") {
            newRes = mImages[name] = new ImageResource();
        } else if(resType == "texture") {
            newRes = mTextures[name] = new TextureResource();
        } else if(resType == "model") {
            newRes = mModels[name] = new ModelResource();
        } else if(resType == "material") {
            newRes = mMaterials[name] = new MaterialResource();
        } else if(resType == "geometry") {
            newRes = mGeometries[name] = new GeometryResource();
        } else if(resType == "font") {
            newRes = mFonts[name] = new FontResource();
        } else {
            newRes = mMiscs[name] = new MiscResource();
        }
        
        newRes->setName(name);
        newRes->setFile(dataPackDir / file);
        newRes->setSize(size);
    }
}

void ResourceManager::preloadAddon(boost::filesystem::path package) {
    Json::Value dataPackData;
    
    {
        std::ifstream reader((package / "data.package").string().c_str());
        reader >> dataPackData;
        reader.close();
    }
    
    Addon* addon = new Addon();
    
    Json::Value& jInfo = dataPackData["info"];
    
    if(!jInfo.isNull()) {
        Json::Value& jName = jInfo["name"];
        Json::Value& jDesc = jInfo["description"];
        Json::Value& jAuthor = jInfo["author"];
        Json::Value& jLicense = jInfo["license"];
        
        if(!jName.isNull()) {
            addon->mName = jName.asString();
        }
        if(!jDesc.isNull()) {
            addon->mDesc = jDesc.asString();
        }
        if(!jAuthor.isNull()) {
            addon->mAuthor = jAuthor.asString();
        }
        if(!jLicense.isNull()) {
            addon->mLicense = jLicense.asString();
        }
    }
    
    Json::Value& jEnviron = dataPackData["environment"];
    if(!jEnviron.isNull()) {
        Json::Value& jAddress = jEnviron["address"];
        Json::Value& jShare = jEnviron["share"];
        Json::Value& jRequire = jEnviron["requre"];
        Json::Value& jAfter = jEnviron["after"];
        
        if(!jAddress.isNull()) {
            addon->mAddress = jAddress.asString();
        }
        if(!jShare.isNull() && jShare.isArray()) {
            for(Json::Value::iterator iter = jShare.begin(); iter != jShare.end(); ++ iter) {
                addon->mShare.push_back(iter->asString());
                std::sort(addon->mShare.begin(), addon->mShare.end());
            }
        }
        if(!jRequire.isNull() && jRequire.isArray()) {
            for(Json::Value::iterator iter = jRequire.begin(); iter != jRequire.end(); ++ iter) {
                addon->mRequire.push_back(iter->asString());
                std::sort(addon->mRequire.begin(), addon->mRequire.end());
            }
        }
        if(!jAfter.isNull() && jAfter.isArray()) {
            for(Json::Value::iterator iter = jAfter.begin(); iter != jAfter.end(); ++ iter) {
                addon->mAfter.push_back(iter->asString());
                std::sort(addon->mAfter.begin(), addon->mAfter.end());
            }
        }
    }
    
    Json::Value& jBootstrap = dataPackData["bootstrap"];
    if(!jBootstrap.isNull() && jBootstrap.isArray()) {
        for(Json::Value::iterator iter = jBootstrap.begin(); iter != jBootstrap.end(); ++ iter) {
            addon->mBootstap.push_back(iter->asString());
        }
    }
    
    mAddons.push_back(addon);
}

void ResourceManager::bootstrapAddons() {
    // Check for address naming conflicts
    {
        typedef std::map<std::string, std::vector<Addon*>> Population;
        
        Population populated;
        bool namingConflict = false;
        for(std::vector<Addon*>::iterator iter = mAddons.begin(); iter != mAddons.end(); ++ iter) {
            Addon* addon = *iter;
            
            if(populated.find(addon->mAddress) == populated.end()) {
                std::vector<Addon*> occupants;
                occupants.push_back(addon);
                populated[addon->mAddress] = occupants;
            }
            else {
                populated[addon->mAddress].push_back(addon);
                namingConflict = true;
            }
        }
        
        if(namingConflict) {
            for(Population::iterator iter = populated.begin(); iter != populated.end(); ++ iter) {
                std::vector<Addon*>& occupants = iter->second;
                if(occupants.size() > 1) {
                    AddonError ae;
                    ae.mType = AddonError::Type::ADDRESS_CONFLICT;
                    ae.mAddons = occupants;
                    
                    for(std::vector<Addon*>::iterator iter2 = occupants.begin(); iter2 != occupants.end(); ++ iter2) {
                        Addon* conflict = *iter2;
                        conflict->mLoadErrors.push_back(ae);
                        
                        // Remove failed addon from list
                        /*
                        for(std::vector<Addon*>::iterator iter3 = mAddons.begin(); iter3 != mAddons.end(); iter3 != mAddons.end()) {
                            Addon* asdf = *iter3;
                            if(conflict == asdf) {
                                mAddons.erase(iter3);
                                break;
                            }
                        }
                        */
                    }
                }
            }
        }
        
    }
    
    // Check for missing requirements
    {
        std::vector<std::string> nonError;
        for(std::vector<Addon*>::iterator iter = mAddons.begin(); iter != mAddons.end();) {
            Addon* addon = *iter;
            
            if(addon->mLoadErrors.size() == 0) {
                nonError.push_back(addon->mAddress);
            }
        }
        std::sort(nonError.begin(), nonError.end());
        
        for(std::vector<Addon*>::iterator iter = mAddons.begin(); iter != mAddons.end(); ++ iter) {
            Addon* addon = *iter;
            if(!std::includes(nonError.begin(), nonError.end(), addon->mRequire.begin(), addon->mRequire.end())) {
                AddonError ae;
                ae.mType = AddonError::Type::MISSING_REQUIREMENT;
                for(std::vector<std::string>::iterator iter2 = addon->mRequire.begin(); iter2 != addon->mRequire.end(); ++ iter2) {
                    std::string requirement = *iter2;
                    if(std::find(nonError.begin(), nonError.end(), requirement) == nonError.end()) {
                        ae.mStrings.push_back(requirement);
                    }
                }
                
                addon->mLoadErrors.push_back(ae);
            }
        }
    }
    
    // Fail addons
    {
        for(std::vector<Addon*>::iterator iter = mAddons.begin(); iter != mAddons.end();) {
            Addon* addon = *iter;
            
            if(addon->mLoadErrors.size() > 0) {
                mFailedAddons.push_back(addon);
                iter = mAddons.erase(iter);
            } else {
                ++ iter;
            }
        }
    }
    
    // Determine load order based on "after"
    typedef std::vector<std::vector<Addon*>> LoadOrder;
    
    {
    }
    
}

void ResourceManager::grabFallbacks() {
    assert(!mFallbacksGrabbed && "Grabbed fallbacks more than once!");
    
    mFallbackString->grab();
    mFallbackImage->grab();
    mFallbackTexture->grab();
    mFallbackModel->grab();
    mFallbackMaterial->grab();
    mFallbackGeometry->grab();
    mFallbackShader->grab();
    mFallbackShaderProgram->grab();
    mFallbackFont->grab();
    
    mFallbacksGrabbed = true;
}

StringResource* ResourceManager::findString(std::string name) {
    StringResource* res = mStrings[name];
    if(!res) {
        std::cout << "Could not find string [" << name << "]" << std::endl;
        return mFallbackString;
    } else {
        return res;
    }
}
ImageResource* ResourceManager::findImage(std::string name) {
    ImageResource* res = mImages[name];
    if(!res) {
        std::cout << "Could not find image [" << name << "]" << std::endl;
        return mFallbackImage;
    } else {
        return res;
    }
}
TextureResource* ResourceManager::findTexture(std::string name) {
    TextureResource* res = mTextures[name];
    if(!res) {
        std::cout << "Could not find texture [" << name << "]" << std::endl;
        return mFallbackTexture;
    } else {
        return res;
    }
}
ModelResource* ResourceManager::findModel(std::string name) {
    ModelResource* res = mModels[name];
    if(!res) {
        std::cout << "Could not find model [" << name << "]" << std::endl;
        return mFallbackModel;
    } else {
        return res;
    }
}
MaterialResource* ResourceManager::findMaterial(std::string name) {
    MaterialResource* res = mMaterials[name];
    if(!res) {
        std::cout << "Could not find material [" << name << "]" << std::endl;
        return mFallbackMaterial;
    } else {
        return res;
    }
}
GeometryResource* ResourceManager::findGeometry(std::string name) {
    GeometryResource* res = mGeometries[name];
    if(!res) {
        std::cout << "Could not find geometry [" << name << "]" << std::endl;
        return mFallbackGeometry;
    } else {
        return res;
    }
}
ShaderResource* ResourceManager::findShader(std::string name) {
    ShaderResource* res = mShaders[name];
    if(!res) {
        std::cout << "Could not find shader [" << name << "]" << std::endl;
        return mFallbackShader;
    } else {
        return res;
    }
}
ShaderProgramResource* ResourceManager::findShaderProgram(std::string name) {
    ShaderProgramResource* res = mShaderPrograms[name];
    if(!res) {
        std::cout << "Could not find shader program [" << name << "]" << std::endl;
        return mFallbackShaderProgram;
    } else {
        return res;
    }
}
FontResource* ResourceManager::findFont(std::string name) {
    FontResource* res = mFonts[name];
    if(!res) {
        std::cout << "Could not find font [" << name << "]" << std::endl;
        return mFallbackFont;
    } else {
        return res;
    }
}

}
