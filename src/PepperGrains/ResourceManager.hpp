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

#ifndef PGG_RESOURCEMANAGER_HPP
#define PGG_RESOURCEMANAGER_HPP

#include <map>

#include <boost/filesystem.hpp>

#include "ImageResource.hpp"
#include "MaterialResource.hpp"
#include "MiscResource.hpp"
#include "ModelResource.hpp"
#include "Resource.hpp"
#include "StringResource.hpp"
#include "TextureResource.hpp"
#include "GeometryResource.hpp"
#include "ShaderResource.hpp"
#include "ShaderProgramResource.hpp"
#include "FontResource.hpp"

namespace pgg {

class ResourceManager {
public:
    static ResourceManager* getSingleton();
private:
    std::map<std::string, MiscResource*> mMiscs;
    
    std::map<std::string, StringResource*> mStrings;
    std::map<std::string, ImageResource*> mImages;
    std::map<std::string, TextureResource*> mTextures;
    std::map<std::string, ModelResource*> mModels;
    std::map<std::string, MaterialResource*> mMaterials;
    std::map<std::string, GeometryResource*> mGeometries;
    std::map<std::string, ShaderResource*> mShaders;
    std::map<std::string, ShaderProgramResource*> mShaderPrograms;
    std::map<std::string, FontResource*> mFonts;
    
    bool mFallbacksGrabbed;
    StringResource* mFallbackString;
    ImageResource* mFallbackImage;
    TextureResource* mFallbackTexture;
    ModelResource* mFallbackModel;
    MaterialResource* mFallbackMaterial;
    GeometryResource* mFallbackGeometry;
    ShaderResource* mFallbackShader;
    ShaderProgramResource* mFallbackShaderProgram;
    FontResource* mFallbackFont;

    uint32_t mPermaloadThreshold;

public:
    ResourceManager();
    ~ResourceManager();

    void setPermaloadThreshold(uint32_t size);
    const uint32_t& getPermaloadThreshold();

    void mapAll(boost::filesystem::path data);
    void grabFallbacks();
    
    StringResource* getFallbackString();
    ImageResource* getFallbackImage();
    TextureResource* getFallbackTexture();
    ModelResource* getFallbackModel();
    MaterialResource* getFallbackMaterial();
    GeometryResource* getFallbackGeometry();
    ShaderResource* getFallbackShader();
    ShaderProgramResource* getFallbackShaderProgram();
    FontResource* getFallbackFont();

    StringResource* findString(std::string name);
    ImageResource* findImage(std::string name);
    TextureResource* findTexture(std::string name);
    ModelResource* findModel(std::string name);
    MaterialResource* findMaterial(std::string name);
    GeometryResource* findGeometry(std::string name);
    ShaderResource* findShader(std::string name);
    ShaderProgramResource* findShaderProgram(std::string name);
    FontResource* findFont(std::string name);
};

}

#endif // RESOURCEMANAGER_HPP


