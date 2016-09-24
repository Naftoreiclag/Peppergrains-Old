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
: mAddonsLoaded(false) {
    mFallbackString = new StringResource();
    mFallbackImage = new ImageResource();
    mFallbackTexture = new TextureResource();
    mFallbackModel = new ModelResource();
    mFallbackMaterial = new MaterialResource();
    mFallbackGeometry = new GeometryResource();
    mFallbackShader = new ShaderResource(ShaderResource::Type::FRAGMENT);
    mFallbackShaderProgram = new ShaderProgramResource();
    mFallbackFont = new FontResource();
    
    /*
    std::cout << "a";
    mFallbackString->grab();
    std::cout << "a";
    mFallbackImage->grab();
    std::cout << "a";
    mFallbackTexture->grab();
    std::cout << "a";
    mFallbackModel->grab();
    std::cout << "a";
    mFallbackMaterial->grab();
    std::cout << "a";
    mFallbackGeometry->grab();
    std::cout << "a";
    mFallbackShader->grab();
    std::cout << "a";
    mFallbackShaderProgram->grab();
    std::cout << "a";
    mFallbackFont->grab();
    std::cout << "a";
    */
}

ResourceManager::~ResourceManager() {
    /*
    mFallbackString->drop();
    mFallbackImage->drop();
    mFallbackTexture->drop();
    mFallbackModel->drop();
    mFallbackMaterial->drop();
    mFallbackGeometry->drop();
    mFallbackShader->drop();
    mFallbackShaderProgram->drop();
    mFallbackFont->drop();
    */
    
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

void ResourceManager::loadCore(boost::filesystem::path package, ScriptEvaluator* evalulator) {
    
}

void ResourceManager::preloadAddon(boost::filesystem::path packageDir) {
    if(!boost::filesystem::exists(packageDir)) return;
    
    Json::Value jPackage;
    
    {
        std::ifstream reader((packageDir / "data.package").string().c_str());
        reader >> jPackage;
        reader.close();
    }
    
    Addon* addon = new Addon();
    
    Json::Value& jInfo = jPackage["info"];
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
    
    Json::Value& jEnviron = jPackage["environment"];
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
    
    Json::Value& jBootstrap = jPackage["bootstrap"];
    if(!jBootstrap.isNull() && jBootstrap.isArray()) {
        for(Json::Value::iterator iter = jBootstrap.begin(); iter != jBootstrap.end(); ++ iter) {
            addon->mBootstap.push_back(iter->asString());
        }
    }
    
    // TODO: check validity of resource list in next loop
    
    const Json::Value& jResources = jPackage["resources"];
    for(Json::Value::const_iterator iter = jResources.begin(); iter != jResources.end(); ++ iter) {
        const Json::Value& jResource = *iter;
        
        std::string resType = jResource["type"].asString();
        std::string name = jResource["name"].asString();
        std::string file = jResource["file"].asString();
        uint32_t size = jResource["size"].asInt();
        
        Resource* newRes;
        if(resType == "text") {
            newRes = addon->mStrings[name] = new StringResource();
        } else if(resType == "compute-shader") {
            newRes = addon->mShaders[name] = new ShaderResource(ShaderResource::Type::COMPUTE);
        } else if(resType == "vertex-shader") {
            newRes = addon->mShaders[name] = new ShaderResource(ShaderResource::Type::VERTEX);
        } else if(resType == "tess-control-shader") {
            newRes = addon->mShaders[name] = new ShaderResource(ShaderResource::Type::TESS_CONTROL);
        } else if(resType == "tess-evaluation-shader") {
            newRes = addon->mShaders[name] = new ShaderResource(ShaderResource::Type::TESS_EVALUATION);
        } else if(resType == "geometry-shader") {
            newRes = addon->mShaders[name] = new ShaderResource(ShaderResource::Type::GEOMETRY);
        } else if(resType == "fragment-shader") {
            newRes = addon->mShaders[name] = new ShaderResource(ShaderResource::Type::FRAGMENT);
        } else if(resType == "shader-program") {
            newRes = addon->mShaderPrograms[name] = new ShaderProgramResource();
        } else if(resType == "image") {
            newRes = addon->mImages[name] = new ImageResource();
        } else if(resType == "texture") {
            newRes = addon->mTextures[name] = new TextureResource();
        } else if(resType == "model") {
            newRes = addon->mModels[name] = new ModelResource();
        } else if(resType == "material") {
            newRes = addon->mMaterials[name] = new MaterialResource();
        } else if(resType == "geometry") {
            newRes = addon->mGeometries[name] = new GeometryResource();
        } else if(resType == "font") {
            newRes = addon->mFonts[name] = new FontResource();
        } else if(resType == "waveform") {
            //newRes = addon->mWaveforms[name] = new WaveformResource();
        } else if(resType == "script") {
            newRes = addon->mScripts[name] = new ScriptResource();
        } else if(resType == "component") {
            //newRes = addon->mComponents[name] = new ComponentResource();
        } else if(resType == "composition") {
            //newRes = addon->mCompositions[name] = new CompositionResource();
        } else {
            newRes = addon->mMiscs[name] = new MiscResource();
        }
        
        newRes->setName(name);
        newRes->setFile(packageDir / file);
        newRes->setSize(size);
    }
    
    mAddons.push_back(addon);
}

void ResourceManager::preloadAddonDirectory(boost::filesystem::path dir) {
    if(!boost::filesystem::exists(dir)) return;
    
    std::vector<boost::filesystem::path> packages;
    {
        boost::filesystem::directory_iterator endIter;
        for(boost::filesystem::directory_iterator iter(dir); iter != endIter; ++ iter) {
            boost::filesystem::path juliet = *iter;
            if(boost::filesystem::is_directory(juliet)) {
                if(boost::filesystem::exists(juliet / "data.package")) {
                    packages.push_back(juliet);
                }
            }
            else {
                if(juliet.has_filename() && juliet.extension() == ".addon") {
                    packages.push_back(juliet);
                }
            }
        }
    }
    
    for(std::vector<boost::filesystem::path>::iterator iter = packages.begin(); iter != packages.end(); ++ iter) {
        preloadAddon(*iter);
    }
}

void ResourceManager::bootstrapAddons(ScriptEvaluator* evalulator) {
    assert(!mAddonsLoaded && "Addons have already been loaded!");
    
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
                    }
                }
            }
        }
        
    }
    
    // Check for missing requirements
    {
        std::vector<std::string> nonError;
        for(std::vector<Addon*>::iterator iter = mAddons.begin(); iter != mAddons.end(); ++ iter) {
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
                ae.mType = AddonError::Type::REQUIREMENT_MISSING;
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
        for(std::vector<Addon*>::iterator iter = mAddons.begin(); iter != mAddons.end(); /*May erase*/) {
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
    std::vector<std::vector<Addon*>> loadOrder;
    
    {
        // First link the "after" set together
        for(std::vector<Addon*>::iterator iter = mAddons.begin(); iter != mAddons.end(); ++ iter) {
            Addon* addon = *iter;
            for(std::vector<std::string>::iterator iter2 = addon->mAfter.begin(); iter2 != addon->mAfter.end(); ++ iter2) {
                std::string afterName = *iter2;
                for(std::vector<Addon*>::iterator iter3 = mAddons.begin(); iter3 != mAddons.end(); ++ iter3) {
                    Addon* other = *iter3;
                    if(other->mAddress == afterName) {
                        addon->mAfterLink.push_back(other);
                        break;
                    }
                }
            }
        }
        
        // Simulate the loading sequence; this vector stores all addons which are currently loaded
        std::vector<Addon*> areLoaded;
        
        // Addons that are not sorted yet
        std::vector<Addon*> yetUnsorted = mAddons;
        
        while(true) {
            // These addons will be loaded together
            std::vector<Addon*> loadGroup;
            
            for(std::vector<Addon*>::iterator iter = yetUnsorted.begin(); iter != yetUnsorted.end(); /*May erase*/) {
                Addon* addon = *iter;
                
                // If all "after" addons have already been loaded, then add this one to the load stack
                bool canLoad = true;
                for(std::vector<Addon*>::iterator iter3 = addon->mAfterLink.begin(); iter3 != addon->mAfterLink.end(); ++ iter3) {
                    Addon* afterMe = *iter3;
                    if(std::find(areLoaded.begin(), areLoaded.end(), afterMe) == areLoaded.end()) {
                        canLoad = false;
                        break;
                    }
                }
                
                if(canLoad) {
                    // Note: Cannot add to areLoaded list yet!
                    loadGroup.push_back(addon);
                    
                    iter = yetUnsorted.erase(iter);
                } else {
                    ++ iter;
                }
            }
            
            if(loadGroup.size() > 0) {
                loadOrder.push_back(loadGroup);
                
                // These are now considered "loaded"
                areLoaded.insert(areLoaded.end(), loadGroup.begin(), loadGroup.end());
            } else {
                // All addons which can be loaded are loaded, leaving only circular dependencies
                if(yetUnsorted.size() > 0) {
                    for(std::vector<Addon*>::iterator iter = yetUnsorted.begin(); iter != yetUnsorted.end(); ++ iter) {
                        Addon* addon = *iter;
                        AddonError ae;
                        ae.mType = AddonError::Type::CIRCULAR_AFTER;
                        
                        // Add to the error list addons which are not yet loaded (and must therefore also have circular dependencies)
                        for(std::vector<Addon*>::iterator iter3 = addon->mAfterLink.begin(); iter3 != addon->mAfterLink.end(); ++ iter3) {
                            Addon* afterMe = *iter3;
                            if(std::find(areLoaded.begin(), areLoaded.end(), afterMe) == areLoaded.end()) {
                                ae.mAddons.push_back(afterMe);
                            }
                        }
                        
                        addon->mLoadErrors.push_back(ae);
                        
                        // Add to fail list, remove from success list
                        mFailedAddons.push_back(addon);
                        mAddons.erase(std::remove(mAddons.begin(), mAddons.end(), addon), mAddons.end());
                    }
                }
                
                // All is well
                else {
                    break;
                }
            }
        }
        
    }
    
    // Load order now set, finally can load
    for(std::vector<std::vector<Addon*>>::iterator iter = loadOrder.begin(); iter != loadOrder.end(); ++ iter) {
        // Particular step
        bootstrapAddonsConcurrently(*iter, evalulator);
    }
    
    // Fail addons that encountered an error during bootstrap
    {
        for(std::vector<Addon*>::iterator iter = mAddons.begin(); iter != mAddons.end(); /*May erase*/) {
            Addon* addon = *iter;
            
            if(addon->mLoadErrors.size() > 0) {
                mFailedAddons.push_back(addon);
                iter = mAddons.erase(iter);
            } else {
                ++ iter;
            }
        }
    }
    
    mAddonsLoaded = true;
}

void ResourceManager::bootstrapAddonsConcurrently(std::vector<Addon*> addons, ScriptEvaluator* eval) {
    // TODO: randomize reading of input vector
    
    for(std::vector<Addon*>::iterator iter = addons.begin(); iter != addons.end(); /*May erase*/) {
        Addon* addon = *iter;
        
        for(std::vector<std::string>::iterator iter2 = addon->mBootstap.begin(); iter2 != addon->mBootstap.end(); ++ iter2) {
            std::string bootName = *iter2;
            
            std::map<std::string, ScriptResource*>::iterator iter3 = addon->mScripts.find(bootName);
            if(iter3 == addon->mScripts.end()) {
                AddonError ae;
                ae.mType = AddonError::BOOTSTRAP_SCRIPT_MISSING;
                ae.mStrings.push_back(bootName);
                
                addon->mLoadErrors.push_back(ae);
                break;
            } else {
                ScriptResource* script = iter3->second;
                
                // Run script
                eval->execute(addon, script);
            }
        }
        
        if(addon->mLoadErrors.size() > 0) {
            mFailedAddons.push_back(addon);
            iter = mAddons.erase(iter);
        } else {
            ++ iter;
        }
    }
}

void ResourceManager::clearAddons() {
    assert(mAddonsLoaded && "Addons have not yet been loaded!");
    
    mAddonsLoaded = false;
}

StringResource* ResourceManager::findString(std::string name, std::string address) {
    StringResource* res = mStrings[name];
    if(!res) {
        std::cout << "Could not find string [" << name << "]" << std::endl;
        return mFallbackString;
    } else {
        return res;
    }
}
ImageResource* ResourceManager::findImage(std::string name, std::string address) {
    ImageResource* res = mImages[name];
    if(!res) {
        std::cout << "Could not find image [" << name << "]" << std::endl;
        return mFallbackImage;
    } else {
        return res;
    }
}
TextureResource* ResourceManager::findTexture(std::string name, std::string address) {
    TextureResource* res = mTextures[name];
    if(!res) {
        std::cout << "Could not find texture [" << name << "]" << std::endl;
        return mFallbackTexture;
    } else {
        return res;
    }
}
ModelResource* ResourceManager::findModel(std::string name, std::string address) {
    ModelResource* res = mModels[name];
    if(!res) {
        std::cout << "Could not find model [" << name << "]" << std::endl;
        return mFallbackModel;
    } else {
        return res;
    }
}
MaterialResource* ResourceManager::findMaterial(std::string name, std::string address) {
    MaterialResource* res = mMaterials[name];
    if(!res) {
        std::cout << "Could not find material [" << name << "]" << std::endl;
        return mFallbackMaterial;
    } else {
        return res;
    }
}
GeometryResource* ResourceManager::findGeometry(std::string name, std::string address) {
    GeometryResource* res = mGeometries[name];
    if(!res) {
        std::cout << "Could not find geometry [" << name << "]" << std::endl;
        return mFallbackGeometry;
    } else {
        return res;
    }
}
ShaderResource* ResourceManager::findShader(std::string name, std::string address) {
    ShaderResource* res = mShaders[name];
    if(!res) {
        std::cout << "Could not find shader [" << name << "]" << std::endl;
        return mFallbackShader;
    } else {
        return res;
    }
}
ShaderProgramResource* ResourceManager::findShaderProgram(std::string name, std::string address) {
    ShaderProgramResource* res = mShaderPrograms[name];
    if(!res) {
        std::cout << "Could not find shader program [" << name << "]" << std::endl;
        return mFallbackShaderProgram;
    } else {
        return res;
    }
}
FontResource* ResourceManager::findFont(std::string name, std::string address) {
    FontResource* res = mFonts[name];
    if(!res) {
        std::cout << "Could not find font [" << name << "]" << std::endl;
        return mFallbackFont;
    } else {
        return res;
    }
}

ScriptResource* ResourceManager::findScript(std::string name, std::string address) {
    
    
}

}
