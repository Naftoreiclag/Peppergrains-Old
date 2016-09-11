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
#include <string>

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

    //
    struct Addon;
    struct AddonError {
        enum Type {
            CIRCULAR_AFTER, // Multiple mods try to load after each other
            BOOTSTRAP_ERROR, // Problem running bootstrap script
            ADDRESS_CONFLICT, // Multiple mods try to occupy the same address
            CORRUPT_MISING_RESOURCE,
            MISSING_REQUIREMENT // Mod listed in "require" absent
        };
        Type mType;
        std::vector<std::string> mStrings;
        std::vector<Addon*> mAddons;
    };

    //
    struct Addon {
        std::vector<AddonError> mLoadErrors;
        
        std::string mName;
        std::string mDesc;
        std::string mAuthor;
        std::string mLicense;
        
        std::string mAddress;
        std::vector<std::string> mShare;
        std::vector<std::string> mRequire;
        std::vector<std::string> mAfter;
        std::vector<std::string> mBootstap;
        
        // Resources provided by this addon
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
    };
    
    std::vector<Addon*> mCores;
    std::vector<Addon*> mAddons;
    std::vector<Addon*> mFailedAddons;
    
    bool mAddonsLoaded;
    
    // Used only when resource lookup fails
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
public:
    ResourceManager();
    ~ResourceManager();
    
    void loadCore(boost::filesystem::path package);
    void preloadAddon(boost::filesystem::path package);
    void bootstrapAddons();
    void clearAddons();

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


