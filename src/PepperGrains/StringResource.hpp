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

#ifndef PGG_STRINGRESOURCE_HPP
#define PGG_STRINGRESOURCE_HPP

#include "Resource.hpp"

namespace pgg {

class StringResource : public Resource {
private:
    std::string mString;
    bool mLoaded;
public:
    StringResource();
    virtual ~StringResource();
    
    
    void load();
    void unload();
    
    const std::string& getString();

};

}

#endif // STRINGRESOURCE_HPP
