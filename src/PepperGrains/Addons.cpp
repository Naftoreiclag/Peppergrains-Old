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
        
        // TODO: check validity of resource list in next loop
        
        const Json::Value& jResources = jPackage["resources"];
        
        Resources::populateResourceMap(addon->mResources, jResources, packageDir);
        
        //mAddons.push_back(addon);
    }
    void preloadAddonDirectory(std::string dir); // Utility; load from directory

    // Load all preloaded addons, running bootstrap scripts. Populates mFailedAddons.
    void bootstrapAddons();

    // Unload all addons, restore core resources to original state.
    void clearAddons();



} // Addons
} // pgg

