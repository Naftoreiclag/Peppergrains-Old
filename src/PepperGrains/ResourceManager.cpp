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

#include <fstream>
#include <iostream>

#include "json/json.h"

namespace pgg {

ResourceManager* ResourceManager::getSingleton() {
    static ResourceManager instance;

    return &instance;
}

ResourceManager::ResourceManager()
: mPermaloadThreshold(0) {
}

ResourceManager::~ResourceManager() {
}

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

StringResource* ResourceManager::findString(std::string name) {
    StringResource* ret = mStrings[name];
    if(!ret) {
        std::cout << "String error" << std::endl;
    }
    return ret;
}
ImageResource* ResourceManager::findImage(std::string name) {
    return mImages[name];
}
TextureResource* ResourceManager::findTexture(std::string name) {
    return mTextures[name];
}
ModelResource* ResourceManager::findModel(std::string name) {
    return mModels[name];
}
MaterialResource* ResourceManager::findMaterial(std::string name) {
    return mMaterials[name];
}
GeometryResource* ResourceManager::findGeometry(std::string name) {
    return mGeometries[name];
}
ShaderResource* ResourceManager::findShader(std::string name) {
    ShaderResource* ret = mShaders[name];
    if(!ret) {
        std::cout << "Shader error" << std::endl;
    }
    return ret;
}
ShaderProgramResource* ResourceManager::findShaderProgram(std::string name) {
    ShaderProgramResource* ret = mShaderPrograms[name];
    if(!ret) {
        std::cout << "Shader program error" << std::endl;
        std::cout << name << std::endl;
    }
    return ret;
}
FontResource* ResourceManager::findFont(std::string name) {
    return mFonts[name];
}

}
