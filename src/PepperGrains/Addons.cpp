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

#include <vector>

#include "json/json.h"
#include "boost/filesystem.hpp"

#include "ResourcesUtil.hpp"
#include "Logger.hpp"

namespace pgg {
namespace Addons {
    
    std::vector<Addon*> mPreloadAddons; // Addons in the preload stage waiting to be bootstrapped
    std::vector<Addon*> mLoadedAddons; // Addons successfully bootstrapped and added to the Resource Modlayer stack
    std::vector<Addon*> mFailedAddons; // Addons that encountered a fatal error during bootstrapping

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
    void bootstrapAddonsConcurrently(std::vector<Addon*> addons) {
        Logger::Out infoLog = Logger::log(Logger::INFO);
        infoLog << "Boot addons concurrently:";
        for(std::vector<Addon*>::iterator iter = addons.begin(); iter != addons.end(); ++ iter) {
            infoLog << " [" << (*iter)->mName << "]";
        }
        infoLog << std::endl;
    }

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
            std::vector<std::string> nonError;
            for(std::vector<Addon*>::iterator iter = mPreloadAddons.begin(); iter != mPreloadAddons.end(); ++ iter) {
                Addon* addon = *iter;
                
                if(addon->mLoadErrors.size() == 0) {
                    nonError.push_back(addon->mAddress);
                }
            }
            std::sort(nonError.begin(), nonError.end());
            
            for(std::vector<Addon*>::iterator iter = mPreloadAddons.begin(); iter != mPreloadAddons.end(); ++ iter) {
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
        
        // Determine load order based on "after"
        std::vector<std::vector<Addon*>> loadOrder;
        {
            // First link the "after" set together
            for(std::vector<Addon*>::iterator iter = mPreloadAddons.begin(); iter != mPreloadAddons.end(); ++ iter) {
                Addon* addon = *iter;
                for(std::vector<std::string>::iterator iter2 = addon->mAfter.begin(); iter2 != addon->mAfter.end(); ++ iter2) {
                    std::string afterName = *iter2;
                    for(std::vector<Addon*>::iterator iter3 = mPreloadAddons.begin(); iter3 != mPreloadAddons.end(); ++ iter3) {
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
        for(std::vector<std::vector<Addon*>>::iterator iter = loadOrder.begin(); iter != loadOrder.end(); ++ iter) {
            // Particular step
            bootstrapAddonsConcurrently(*iter);
        }
        
        // Fail addons that encountered an error during bootstrap
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
        
        mLoadedAddons.insert(mLoadedAddons.end(), mPreloadAddons.begin(), mPreloadAddons.end());
        mPreloadAddons.clear();
    }

    // Unload all addons, restore core resources to original state.
    void clearAddons() {
        
    }
    
    std::vector<Addon*> getFailedAddons();
    void clearFailedAddons();



} // Addons
} // pgg

