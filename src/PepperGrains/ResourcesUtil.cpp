/*
   Copyright 2016-2017 James Fong

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

#include "ResourcesUtil.hpp"

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
#include "Logger.hpp"

namespace pgg {
namespace Resources {
    
    void populateResourceMap(ResourceMap& sResources, const Json::Value& resourcesData, boost::filesystem::path dataPackDir) {
        for(Json::Value::const_iterator iter = resourcesData.begin(); iter != resourcesData.end(); ++ iter) {
            const Json::Value& resourceData = *iter;
            
            std::string resType = resourceData["type"].asString();
            std::string name = resourceData["name"].asString();
            std::string file = resourceData["file"].asString();
            uint32_t size = resourceData["size"].asInt();
            
            Resource* newRes;
            if(resType == "string") {
                newRes = new StringResource();
            } else if(resType == "compute-shader") {
                newRes = new ShaderResource(ShaderResource::Type::COMPUTE);
            } else if(resType == "vertex-shader") {
                newRes = new ShaderResource(ShaderResource::Type::VERTEX);
            } else if(resType == "tess-control-shader") {
                newRes = new ShaderResource(ShaderResource::Type::TESS_CONTROL);
            } else if(resType == "tess-evaluation-shader") {
                newRes = new ShaderResource(ShaderResource::Type::TESS_EVALUATION);
            } else if(resType == "geometry-shader") {
                newRes = new ShaderResource(ShaderResource::Type::GEOMETRY);
            } else if(resType == "fragment-shader") {
                newRes = new ShaderResource(ShaderResource::Type::FRAGMENT);
            } else if(resType == "shader-program") {
                newRes = new ShaderProgramResource();
            } else if(resType == "image") {
                newRes = new ImageResource();
            } else if(resType == "texture") {
                newRes = new TextureResource();
            } else if(resType == "model") {
                newRes = new ModelResource();
            } else if(resType == "material") {
                newRes = new MaterialResource();
            } else if(resType == "geometry") {
                newRes = new GeometryResource();
            } else if(resType == "font") {
                newRes = new FontResource();
            } else if(resType == "script") {
                newRes = new ScriptResource();
            } else {
                newRes = new MiscResource();
            }
            
            newRes->setName(name);
            newRes->setFile(dataPackDir / file);
            newRes->setSize(size);
            
            std::pair<ResourceMap::iterator, bool> success = sResources.insert(std::make_pair(name, newRes));
            if(!success.second) {
                Logger::log(Logger::WARN)
                    << "Multiple resources with name: " << name << std::endl
                    << "Conficts with " << success.first->second->getName() << std::endl;
            }
        }
        
        Logger::log(Logger::INFO) << "Successfully loaded resources from: " << dataPackDir << std::endl;
    }
}
}

