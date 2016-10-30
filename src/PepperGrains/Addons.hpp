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

#include <map>
#include <string>

#include "Resource.hpp"
#include "Scripts.hpp"

/* Handles the loading and unloading of addons.
 * 
 * 
 */

namespace pgg {
namespace Addons {
    
    struct Addon;
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
        
        // Lua env
        Scripts::RegRef mLuaEnv = LUA_NOREF;
        
        // Requested properties
        std::string mAddress;
        std::vector<std::string> mShare;
        std::vector<std::string> mRequire;
        std::vector<std::string> mAfter;
        
        // Bootstrap scripts to run, in order
        std::vector<std::string> mBootstap;
        
        // Links to addons which determined this addon's load order (discludes errors at that point) (includes those implied by the "requre" list)
        std::vector<Addon*> mAfterLink;
        
        // Links to addons which this addon requires (better than "neededBy" because that needs more complicated error checking)
        std::vector<Addon*> mRequireLink;
        
        // Links to addons which have given this addon permission to access protected members
        std::vector<Addon*> mAccessTo;
        
        // Resources provided by this addon
        std::map<std::string, Resource*> mResources;
    };

    // Parse a package and add to the loading list
    void preloadAddon(std::string package);
    void preloadAddonDirectory(std::string dir); // Utility; load from directory

    // Load all preloaded addons, running bootstrap scripts. Populates mFailedAddons.
    void bootstrapAddons();

    // Unload all addons, restore core resources to original state.
    void clearAddons();
    
    void logAddonFailures();
    
    std::vector<Addon*> getFailedAddons();
    void clearFailedAddons();

}
}

#endif // PGG_ENGINEADDONS_HPP
