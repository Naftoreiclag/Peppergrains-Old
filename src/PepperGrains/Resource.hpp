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

#include <boost/filesystem.hpp>

#include "ReferenceCounted.hpp"

namespace pgg {

// Virtual inheritance to avoid diamond conflict with ModelResource
class Resource : virtual public ReferenceCounted {
private:
    uint32_t mFileSize;
    std::string mName;
    boost::filesystem::path mFile;
public:
    Resource();
    virtual ~Resource();
    
    void setFile(const boost::filesystem::path& file);
    const boost::filesystem::path& getFile();
    void setName(std::string name);
    const std::string& getName();
    void setSize(uint32_t size);
    const uint32_t& getSize();
};

}

#endif // RESOURCE_HPP
