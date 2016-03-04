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

#include <cassert>
#include <fstream>
#include <iostream>

#include "json/json.h"

namespace pgg {

ResourceManager* ResourceManager::getSingleton() {
    static ResourceManager instance;

    return &instance;
}

ResourceManager::ResourceManager()
: mPermaloadThreshold(0),
mFallbacksGrabbed(false) {
    mFallbackString = new StringResource();
    mFallbackImage = new ImageResource();
    mFallbackTexture = new TextureResource();
    mFallbackModel = new ModelResource();
    mFallbackMaterial = new MaterialResource();
    mFallbackGeometry = new GeometryResource();
    mFallbackShader = new FragmentShaderResource();
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

void ResourceManager::setPermaloadThreshold(uint32_t size) {
    mPermaloadThreshold = size;
}
const uint32_t& ResourceManager::getPermaloadThreshold() {
    return mPermaloadThreshold;
}

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
        } else if(resType == "vertex-shader") {
            newRes = mShaders[name] = new VertexShaderResource();
        } else if(resType == "fragment-shader") {
            newRes = mShaders[name] = new FragmentShaderResource();
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
        if(size < mPermaloadThreshold) {
            newRes->grab();
        }
        
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
        return mFallbackString;
    } else {
        return res;
    }
}
ImageResource* ResourceManager::findImage(std::string name) {
    ImageResource* res = mImages[name];
    if(!res) {
        return mFallbackImage;
    } else {
        return res;
    }
}
TextureResource* ResourceManager::findTexture(std::string name) {
    TextureResource* res = mTextures[name];
    if(!res) {
        return mFallbackTexture;
    } else {
        return res;
    }
}
ModelResource* ResourceManager::findModel(std::string name) {
    ModelResource* res = mModels[name];
    if(!res) {
        return mFallbackModel;
    } else {
        return res;
    }
}
MaterialResource* ResourceManager::findMaterial(std::string name) {
    MaterialResource* res = mMaterials[name];
    if(!res) {
        return mFallbackMaterial;
    } else {
        return res;
    }
}
GeometryResource* ResourceManager::findGeometry(std::string name) {
    GeometryResource* res = mGeometries[name];
    if(!res) {
        return mFallbackGeometry;
    } else {
        return res;
    }
}
ShaderResource* ResourceManager::findShader(std::string name) {
    ShaderResource* res = mShaders[name];
    if(!res) {
        return mFallbackShader;
    } else {
        return res;
    }
}
ShaderProgramResource* ResourceManager::findShaderProgram(std::string name) {
    ShaderProgramResource* res = mShaderPrograms[name];
    if(!res) {
        return mFallbackShaderProgram;
    } else {
        return res;
    }
}
FontResource* ResourceManager::findFont(std::string name) {
    FontResource* res = mFonts[name];
    if(!res) {
        return mFallbackFont;
    } else {
        return res;
    }
}

}
