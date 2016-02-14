#ifndef STRINGRESOURCE_HPP
#define STRINGRESOURCE_HPP

#include "Resource.hpp"

namespace pgg {

class StringResource : public Resource {
private:
    std::string mString;
    bool mLoaded;
public:
    StringResource();
    virtual ~StringResource();
    
    
    bool load();
    bool unload();
    
    const std::string& getString();

};

}

#endif // STRINGRESOURCE_HPP
