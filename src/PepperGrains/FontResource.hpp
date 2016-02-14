#ifndef PGG_FONTRESOURCE_HPP
#define PGG_FONTRESOURCE_HPP

#include "Resource.hpp"

namespace pgg {

class FontResource : public Resource {
private:
    bool mLoaded;
public:
    FontResource();
    virtual ~FontResource();

    bool load();
    bool unload();
};

}

#endif // PGG_FONTRESOURCE_HPP
