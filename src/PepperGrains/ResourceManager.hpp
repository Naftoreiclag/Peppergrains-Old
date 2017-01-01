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
#include "ScriptResource.hpp"

namespace pgg {

class ResourceManager {
public:
    static ResourceManager* getSingleton();
    
    StringResource* findString(std::string name, std::string address = "core");
    ImageResource* findImage(std::string name, std::string address = "core");
    TextureResource* findTexture(std::string name, std::string address = "core");
    ModelResource* findModel(std::string name, std::string address = "core");
    MaterialResource* findMaterial(std::string name, std::string address = "core");
    GeometryResource* findGeometry(std::string name, std::string address = "core");
    ShaderResource* findShader(std::string name, std::string address = "core");
    ShaderProgramResource* findShaderProgram(std::string name, std::string address = "core");
    FontResource* findFont(std::string name, std::string address = "core");
    ScriptResource* findScript(std::string name, std::string address = "core");
    
    struct Addon;
    
private:
    //
    struct AddonError {
        enum Type {
            // Encountered during load order determination
            CIRCULAR_AFTER, // Multiple addons try to load after each other
            ADDRESS_CONFLICT, // Multiple addons try to occupy the same address
            
            // Problem running bootstrap scripts
            BOOTSTRAP_SCRIPT_ERROR, // Script error during execution
            BOOTSTRAP_SCRIPT_MISSING, // Specified script cannot be found
            
            CONCURRENT_MODIFICATION, // Access racing with another addon
            
            REQUIREMENT_CRASHED, // Addon listed in "require" present, but crashes
            REQUIREMENT_MISSING, // Addon listed in "require" absent
            
            // These can only happen due to failed Resource Manager output
            CORRUPT_MISING_RESOURCE // Addon has a missing resource
        };
        Type mType;
        
        // Displayed to user
        std::vector<std::string> mStrings;
        std::vector<Addon*> mAddons;
    };

public:
    //
    struct Addon {
        std::vector<AddonError> mLoadErrors;
        
        // Human-readable info
        std::string mName;
        std::string mDesc;
        std::string mAuthor;
        std::string mLicense;
        
        // Requested properties
        std::string mAddress;
        std::vector<std::string> mShare;
        std::vector<std::string> mRequire;
        std::vector<std::string> mAfter;
        
        // Bootstrap scripts to run, in order
        std::vector<std::string> mBootstap;
        
        // Links to addons which determined this addon's load order (discludes errors at that point)
        std::vector<Addon*> mAfterLink;
        
        // Links to addons which listed this addon as a requirement
        std::vector<Addon*> mNeededBy;
        
        // Links to addons which have given this addon permission to access protected members
        std::vector<Addon*> mAccessTo;
        
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
        std::map<std::string, ScriptResource*> mScripts;
    };

private:
    std::vector<Addon*> mCores;
    std::vector<Addon*> mAddons;
    std::vector<Addon*> mFailedAddons;
    
    bool mAddonsLoaded;
    
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
    std::map<std::string, ScriptResource*> mScripts;
    
public:
    ResourceManager();
    ~ResourceManager();
    
    // Changes to core resources
    struct CoreModification {
        
    };
    
    // Abstractions are fun
    // Subclass ScriptEvaluator to pass during bootstrapping
    class ScriptEvaluator {
    public:
        virtual void execute(const Addon* addon, ScriptResource* bootScript) = 0;
    };
    
    // Loading of core resources which are never unloaded. Restored to original state if an addon
    // which modifies a core resource is unloaded
    void loadCore(boost::filesystem::path package, ScriptEvaluator* evaluator);
    
    // Parse a package and add to the loading list
    void preloadAddon(boost::filesystem::path package);
    void preloadAddonDirectory(boost::filesystem::path dir); // Utility; load from directory
    
    // Load all preloaded addons, running bootstrap scripts. Populates mFailedAddons.
    void bootstrapAddons(ScriptEvaluator* evalulator);
    
    // Unload all addons, restore core resources to original state.
    void clearAddons();
private:

    
    /* To prevent errors occuring due to arbitrary load order, all addons which are eligible to
     * be loaded at any given point in the load process are loaded "together," i.e. as if they
     * were all loading at the same time on different threads.
     * 
     * This method simulates this effect by loading the addons sequentially, but keeping track of
     * the resources that they modify. If multiple addons try to modify the same resource in a
     * single call of this method, both error.
     * 
     * Unfortunately, there may still be order-dependency which cannot be checked (e.g. a script
     * calling another script's function created within the same call cannot be intervened). To
     * "prevent" this, load order within this call is also randomized.
     * 
     * Error checking is also done on all addons before bootstrapping them "individually"
     */
    void bootstrapAddonsConcurrently(std::vector<Addon*> addons, ScriptEvaluator* evalulator);

    
public:
    
    void mapAll(boost::filesystem::path data);
    
    StringResource* getFallbackString();
    ImageResource* getFallbackImage();
    TextureResource* getFallbackTexture();
    ModelResource* getFallbackModel();
    MaterialResource* getFallbackMaterial();
    GeometryResource* getFallbackGeometry();
    ShaderResource* getFallbackShader();
    ShaderProgramResource* getFallbackShaderProgram();
    FontResource* getFallbackFont();
private:
    
    // Used only when resource lookup fails
    StringResource* mFallbackString;
    ImageResource* mFallbackImage;
    TextureResource* mFallbackTexture;
    ModelResource* mFallbackModel;
    MaterialResource* mFallbackMaterial;
    GeometryResource* mFallbackGeometry;
    ShaderResource* mFallbackShader;
    ShaderProgramResource* mFallbackShaderProgram;
    FontResource* mFallbackFont;
};

}

#endif // RESOURCEMANAGER_HPP


