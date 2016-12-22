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

#include "Resources.hpp"

#include <algorithm>
#include <map>

#include "boost/filesystem.hpp"
#include "json/json.h"

#include "Addons.hpp"
#include "ResourcesUtil.hpp"
#include "Logger.hpp"

namespace pgg {
namespace Resources {
    
    ResourceMap sResources;
    
    uint32_t getNumCoreResources() {
        return sResources.size();
    }
    
    void loadCore(std::string strDataPackFile) {
        boost::filesystem::path dataPackFile(strDataPackFile);
        
        Json::Value dataPackData;
        {
            std::ifstream reader(dataPackFile.string().c_str());
            reader >> dataPackData;
            reader.close();
        }
    
        boost::filesystem::path dataPackDir = dataPackFile.parent_path();
        
        const Json::Value& resourcesData = dataPackData["resources"];
        
        Resources::populateResourceMap(sResources, resourcesData, dataPackDir);
        
        std::string criticalResources[] = {
            ":Error.image",
            ":Error.texture"
        };
        for(std::string resName : criticalResources) {
            if(!Resources::find(resName)) {
                Logger::log(Logger::SEVERE) << "Missing critical resource: [" << resName << "]!" << std::endl;
            }
        }
    }

    // Top modlayer can be edited dynamically
    void setTopModlayer(Modlayer* modlayer) {
        
    }
    void removeTopModlayer() {
        
    }
    void publishTopModlayer() {
        
    }
    void removeAllModlayers() {
        
    }

    Resource* find(std::string id, std::string callOrigin) {
        std::string address;
        auto dot = id.find(':');
        if(dot != std::string::npos) {
            std::string address = id.substr(0, dot);
            id = id.substr(dot + 1);
        } else {
            address = callOrigin;
        }
        // address:id
        
        // Empty addresses search in the "core" resources (i.e. sResources)
        if(address == "") {
            ResourceMap::iterator iter = sResources.find(id);
            if(iter == sResources.end()) {
                Logger::log(Logger::WARN) << "Could not find resource: [:" << id << ']' << std::endl;
                return nullptr;
            }
            
            Logger::log(Logger::VERBOSE) << "Found resource: [:" << id << ']' << std::endl;
            return iter->second;
        }
        
        // Explicit addresses search in addons
        else {
            Addons::Addon* addon = Addons::getTempAddon();
            
            if(!addon) {
                addon = Addons::getAddon(address);
                if(!addon) {
                    Logger::log(Logger::WARN) << "Could not resolve address: [" << address << ']' << std::endl;
                    return nullptr;
                }
            }
            
            auto iter = addon->mResources.find(id);
            if(iter == addon->mResources.end()) {
                Logger::log(Logger::WARN) << "Could not find resource: [" << address << ':' << id << ']' << std::endl;
                return nullptr;
            } else {
                Logger::log(Logger::VERBOSE) << "Found resource: [" << address << ':' << id << ']' << std::endl;
                return iter->second;
            }
        }
    }
}
}

