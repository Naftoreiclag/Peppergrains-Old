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

#ifndef PGG_RESOURCE_HPP
#define PGG_RESOURCE_HPP

#include <stdint.h>

// TODO: remove this include, use strings instead
#include <boost/filesystem.hpp>

#include "ReferenceCounted.hpp"

namespace pgg {

namespace Addons {
    struct Addon;
}

// Virtual inheritance to avoid diamond conflict with ModelResource
class Resource : virtual public ReferenceCounted {
public:
    enum Type {
        IMAGE, // Image
        MATERIAL, // Generic Json
        MODEL, // Generic Json
        SHADER,
        SHADER_PROGRAM, // Generic Json
        STRING,
        TEXTURE, // Generic Json
        GEOMETRY, // Geometry
        FONT, // Font
        WAVEFORM, // Waveform
        SCRIPT,
        COMPONENT,
        COMPOSITION,
        
        OTHER
    };
private:
    uint32_t mFileSize;
    std::string mName;
    boost::filesystem::path mFile;
    Addons::Addon* mAddon;
public:
    Resource(Type resourceType);
    virtual ~Resource();
    
    const Type mResourceType;
    
    void setFile(boost::filesystem::path file);
    boost::filesystem::path getFile() const;
    void setName(std::string name);
    std::string getName() const;
    void setSize(uint32_t size);
    uint32_t getSize() const;
    void setAddon(Addons::Addon* addon);
    Addons::Addon* getAddon() const;
};

}

#endif // RESOURCE_HPP
