/*
   Copyright 2016 James Fong

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

#ifndef PGG_ENGINEADDONS_HPP
#define PGG_ENGINEADDONS_HPP

#include <boost/filesystem.hpp>

/* Handles the loading and unloading of addons.
 * 
 * 
 */

namespace pgg {
namespace Addons {
    /*
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
    */
    // Loading of core resources which are never unloaded. Constitutes the "original state"
    void loadCore(boost::filesystem::path package);

    // Parse a package and add to the loading list
    void preloadAddon(boost::filesystem::path package);
    void preloadAddonDirectory(boost::filesystem::path dir); // Utility; load from directory

    // Load all preloaded addons, running bootstrap scripts. Populates mFailedAddons.
    void bootstrapAddons();

    // Unload all addons, restore core resources to original state.
    void clearAddons();

}
}

#endif // PGG_ENGINEADDONS_HPP
