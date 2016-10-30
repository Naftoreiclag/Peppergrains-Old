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

#include "Addons.hpp"

#include <cassert>
#include <vector>
#include <map>

#include "json/json.h"
#include "boost/filesystem.hpp"

#include "ResourcesUtil.hpp"
#include "Logger.hpp"
#include "ScriptResource.hpp"

namespace pgg {
namespace Addons {
    
    std::vector<Addon*> mPreloadAddons; // Addons in the preload stage waiting to be bootstrapped
    std::map<std::string, Addon*> mLoadedAddons; // Addons successfully bootstrapped and added to the Resource Modlayer stack
    std::vector<Addon*> mFailedAddons; // Addons that encountered a fatal error during bootstrapping
    
    // Addon which is treated as if it were loaded, but really it is not.
    // (used during bootstrapping to allow an addon to access its own resources, even though it technically is not loaded yet.)
    Addon* mTempAddon = nullptr;

    // Parse a package and add to the loading list
    void preloadAddon(std::string strPackageDir) {
        boost::filesystem::path packageDir(strPackageDir);
        
        if(!boost::filesystem::exists(packageDir)) {
            Logger::log(Logger::WARN) << "Addon directory does not exist: " << packageDir << std::endl;
            return;
        }
        
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
        
        const Json::Value& jResources = jPackage["resources"];
        Resources::populateResourceMap(addon->mResources, jResources, packageDir);
        mPreloadAddons.push_back(addon);
    }
    
    void preloadAddonDirectory(std::string strDir) {
        boost::filesystem::path dir(strDir);
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
            preloadAddon((*iter).string());
        }
    }

    // Load all preloaded addons, running bootstrap scripts. Populates mFailedAddons.
    void bootstrapAddons() {
        // Debug information
        Logger::Out dlog = Logger::log(Logger::INFO);
        // Check for address naming conflicts
        {
            typedef std::map<std::string, std::vector<Addon*>> Population;
            
            Population populated;
            bool namingConflict = false;
            for(std::vector<Addon*>::iterator iter = mPreloadAddons.begin(); iter != mPreloadAddons.end(); ++ iter) {
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
            for(auto addonIter = mPreloadAddons.begin(); addonIter != mPreloadAddons.end(); ++ addonIter) {
                Addon* addon = *addonIter;
                
                std::vector<std::string> missingRequirements;
                std::vector<Addon*> brokenRequirements;
                
                for(auto strIter = addon->mRequire.begin(); strIter != addon->mRequire.end(); ++ strIter) {
                    std::string otherAddress = *strIter;
                    bool found = false;
                    for(auto otherIter = mPreloadAddons.begin(); otherIter != mPreloadAddons.end(); ++ otherIter) {
                        Addon* other = *otherIter;
                        if(other->mAddress == otherAddress) {
                            if(other->mLoadErrors.size() > 0) {
                                brokenRequirements.push_back(other);
                            }
                            found = true;
                            break;
                        }
                    }
                    if(!found) {
                        missingRequirements.push_back(otherAddress);
                    }
                }
                
                if(missingRequirements.size() > 0) {
                    AddonError ae;
                    ae.mType = AddonError::Type::REQUIREMENT_MISSING;
                    ae.mStrings = missingRequirements;
                    addon->mLoadErrors.push_back(ae);
                }
                if(brokenRequirements.size() > 0) {
                    AddonError ae;
                    ae.mType = AddonError::Type::REQUIREMENT_CRASHED;
                    ae.mAddons = brokenRequirements;
                    addon->mLoadErrors.push_back(ae);
                }
            }
        }
        
        // Fail addons
        {
            for(std::vector<Addon*>::iterator iter = mPreloadAddons.begin(); iter != mPreloadAddons.end(); /*May erase*/) {
                Addon* addon = *iter;
                
                if(addon->mLoadErrors.size() > 0) {
                    mFailedAddons.push_back(addon);
                    iter = mPreloadAddons.erase(iter);
                } else {
                    ++ iter;
                }
            }
        }
        
        // Determine load order based on "after" and "require"
        std::vector<std::vector<Addon*>> loadOrder;
        {
            for(auto addonIter = mPreloadAddons.begin(); addonIter != mPreloadAddons.end(); ++ addonIter) {
                Addon* addon = *addonIter;
                for(auto otherIter = mPreloadAddons.begin(); otherIter != mPreloadAddons.end(); ++ otherIter) {
                    Addon* other = *otherIter;
                    
                    // Note: both the "after" and "require" lists are already sorted; using binary_search to take advantage of this
                    
                    // This other addon is in the "after" or "require" list
                    if(
                        std::binary_search(addon->mAfter.begin(), addon->mAfter.end(), other->mAddress) || 
                        std::binary_search(addon->mRequire.begin(), addon->mRequire.end(), other->mAddress)) {
                        
                        addon->mAfterLink.push_back(other);
                    }
                }
            }
            
            // Simulate the loading sequence; this vector stores all addons which are currently loaded
            std::vector<Addon*> areLoaded;
            
            // Addons that are not sorted yet
            std::vector<Addon*> yetUnsorted = mPreloadAddons;
            
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
                            mPreloadAddons.erase(std::remove(mPreloadAddons.begin(), mPreloadAddons.end(), addon), mPreloadAddons.end());
                        }
                        break;
                    }
                    
                    // All is well
                    else {
                        break;
                    }
                }
            }
            
        }
        
        // Load order now set, finally can load
        Scripts::enableBootstrap();
        if(loadOrder.size() > 0)
        {
            dlog << "Addon load order: " << std::endl;
            uint32_t debugStep = 1; // What step is being processed
            for(auto stackIter = loadOrder.begin(); stackIter != loadOrder.end(); ++ stackIter) {
                dlog << debugStep << ": ";
                std::vector<Addon*> concurrentAddons = *stackIter;
                std::string sep = "{[";
                for(auto addonIter = concurrentAddons.begin(); addonIter != concurrentAddons.end(); ++ addonIter) {
                    Addon* addon = *addonIter;
                    dlog << sep << addon->mAddress << ']';
                    sep = " [";
                }
                dlog << '}' << std::endl;
                ++ debugStep;
            }
            
            for(auto stackIter = loadOrder.begin(); stackIter != loadOrder.end(); ++ stackIter) {
                // These addons will be loaded concurrently
                std::vector<Addon*> concurrentAddons = *stackIter;
                
                // Try to load these addons as normal
                bool errorsEncounted = false;
                for(auto addonIter = concurrentAddons.begin(); addonIter != concurrentAddons.end(); ++ addonIter) {
                    Addon* addon = *addonIter;
                    
                    // Skip addons that have errored from previous iterations of loadOrder
                    if(addon->mLoadErrors.size() > 0) {
                        errorsEncounted = true;
                        continue;
                    }
                    
                    // Allow future operations to treat this addon as if it were already loaded
                    mTempAddon = addon;
                    
                    // Pre-grab all scripts
                    std::vector<ScriptResource*> runThese;
                    std::vector<std::string> missingScripts;
                    for(auto scrNameIter = addon->mBootstap.begin(); scrNameIter != addon->mBootstap.end(); ++ scrNameIter) {
                        ScriptResource* sres = ScriptResource::upcast(Resources::find(*scrNameIter, addon->mAddress));
                        if(!sres) {
                            missingScripts.push_back(*scrNameIter);
                        } else {
                            sres->grab();
                            runThese.push_back(sres);
                        }
                    }
                    
                    // Error for missing scripts
                    if(missingScripts.size() > 0) {
                        AddonError ae;
                        ae.mType = AddonError::Type::BOOTSTRAP_SCRIPT_MISSING;
                        ae.mStrings = missingScripts;
                        addon->mLoadErrors.push_back(ae);
                        errorsEncounted = true;
                    }
                    
                    // This should never happen
                    assert(addon->mLuaEnv == LUA_NOREF && "Addon already has a Lua environment!");
                    
                    // Create addon environment
                    Scripts::RegRef addonEnv = Scripts::newEnvironment();
                    addon->mLuaEnv = addonEnv;
                    
                    // Set environment for all scripts
                    // TODO: Keep a temporary list of all scripts in the Addon struct
                    for(auto resIter = addon->mResources.begin(); resIter != addon->mResources.end(); ++ resIter) { // Hooray for auto
                        Resource* res = resIter->second;
                        
                        if(res->mResourceType == Resource::Type::SCRIPT) {
                            ScriptResource* sres = ScriptResource::upcast(res);
                            sres->setEnv(addonEnv); // Yes, we can do this before grabbing it
                        }
                    }
                    
                    
                    // Run all scripts (even if some are missing, just to get more crash data) (also need to drop grabs)
                    for(auto scriptIter = runThese.begin(); scriptIter != runThese.end(); ++ scriptIter) {
                        ScriptResource* sres = *scriptIter;
                        Scripts::CallStat cstat = sres->run();
                        if(cstat.mError != Scripts::ERR_OK) {
                            // Error
                            AddonError ae;
                            ae.mType = AddonError::Type::BOOTSTRAP_SCRIPT_ERROR;
                            ae.mStrings.push_back(cstat.mErrorMsg);
                            addon->mLoadErrors.push_back(ae);
                            errorsEncounted = true;
                        }
                        sres->drop();
                    }
                }
                
                mTempAddon = nullptr;
                
                // Check for access racing
                {
                    
                }
                
                // Fail any error'd addons and also fail any addons later in the load order depending on this one
                // Note that this does not and should not add to mFailedAddons (That happens later)
                if(errorsEncounted) {
                    for(auto addonIter = concurrentAddons.begin(); addonIter != concurrentAddons.end(); ++ addonIter) {
                        Addon* addon = *addonIter;
                        if(addon->mLoadErrors.size() > 0) {
                            // Search all later steps and addons within those steps
                            // Error only one layer; future errors will be caught in future iterations
                            for(auto futureStackIter = stackIter + 1; futureStackIter != loadOrder.end(); ++ futureStackIter) {
                                std::vector<Addon*> futureConcurrentAddons = *futureStackIter;
                                for(auto futureAddonIter = futureConcurrentAddons.begin(); futureAddonIter != futureConcurrentAddons.end(); ++ futureAddonIter) {
                                    Addon* futureAddon = *futureAddonIter;
                                    
                                    // This addon requierd *addonIter
                                    if(std::find(futureAddon->mRequireLink.begin(), futureAddon->mRequireLink.end(), addon) != futureAddon->mRequireLink.end()) {
                                        AddonError ae;
                                        ae.mType = AddonError::Type::REQUIREMENT_CRASHED;
                                        ae.mAddons.push_back(addon);
                                        futureAddon->mLoadErrors.push_back(ae);
                                        
                                        // TODO: assert that this dependency has not yet been encountered in the loadOrder stack
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        Scripts::enableBootstrap(false);
        
        // Fail all broken addons
        {
            for(std::vector<Addon*>::iterator iter = mPreloadAddons.begin(); iter != mPreloadAddons.end(); /*May erase*/) {
                Addon* addon = *iter;
                
                if(addon->mLoadErrors.size() > 0) {
                    mFailedAddons.push_back(addon);
                    iter = mPreloadAddons.erase(iter);
                } else {
                    ++ iter;
                }
            }
        }
        
        {
            for(auto iter = mPreloadAddons.begin(); iter != mPreloadAddons.end(); ++ iter) {
                Addon* addon = *iter;
                mLoadedAddons[addon->mAddress] = addon;
            }
            mPreloadAddons.clear();
        }
    }
    
    Addon* getAddon(std::string address) {
        auto iter = mLoadedAddons.find(address);
        if(iter == mLoadedAddons.end()) {
            Logger::log(Logger::WARN) << "Could not addon with address: " << address << std::endl;
            return nullptr;
        } else {
            return iter->second;
        }
    }
    
    Addon* getTempAddon() { return mTempAddon; }

    // Unload all addons, restore core resources to original state.
    void clearAddons() {
        
    }
    
    void logAddonFailures() {
        if(mFailedAddons.size() == 0) {
            Logger::log(Logger::INFO) << "All addons loaded successfully" << std::endl;
            return;
        }
        
        Logger::Out wlog = Logger::log(Logger::WARN);
        
        for(auto failIter = mFailedAddons.begin(); failIter != mFailedAddons.end(); ++ failIter) {
            Addon* addon = *failIter;
            
            assert(addon->mLoadErrors.size() > 0 && "Addon in mFailedAddons with no errors!");
            wlog << "[" << addon->mName << "] failed to load:" << std::endl;
            wlog.indent();
            for(auto error = addon->mLoadErrors.begin(); error != addon->mLoadErrors.end(); ++ error) {
                AddonError& ae = *error;
                
                switch(ae.mType) {
                    case AddonError::Type::ADDRESS_CONFLICT: {
                        wlog << "Address conflict:" << std::endl;
                        break;
                    }
                    case AddonError::Type::BOOTSTRAP_SCRIPT_ERROR: {
                        wlog << "Bootstrap script error:" << std::endl;
                        break;
                    }
                    case AddonError::Type::BOOTSTRAP_SCRIPT_MISSING: {
                        wlog << "Missing named boostrap script:" << std::endl;
                        break;
                    }
                    case AddonError::Type::CIRCULAR_AFTER: {
                        wlog << "Circular \"after\" declarations:" << std::endl;
                        break;
                    }
                    case AddonError::Type::CONCURRENT_MODIFICATION: {
                        wlog << "Access racing:" << std::endl;
                        break;
                    }
                    case AddonError::Type::CORRUPT_MISING_RESOURCE: {
                        wlog << "Corrupt or missing resource:" << std::endl;
                        break;
                    }
                    case AddonError::Type::REQUIREMENT_CRASHED: {
                        wlog << "Required addon(s) failed to load:" << std::endl;
                        break;
                    }
                    case AddonError::Type::REQUIREMENT_MISSING: {
                        wlog << "Missing required addons:" << std::endl;
                        break;
                    }
                }
                
                wlog.indent();
                for(auto iter = ae.mStrings.begin(); iter != ae.mStrings.end(); ++ iter) {
                    wlog << *iter << std::endl;
                }
                for(auto iter = ae.mAddons.begin(); iter != ae.mAddons.end(); ++ iter) {
                    Addon* a = *iter;
                    wlog << '[' << a->mName << ']' << std::endl;
                }
                wlog.unindent();
            }
            wlog.unindent();
        }
    }
    
    std::vector<Addon*> getFailedAddons() { return mFailedAddons; }
    void clearFailedAddons() {
        // TODO: properly call delete's
    }



} // Addons
} // pgg

