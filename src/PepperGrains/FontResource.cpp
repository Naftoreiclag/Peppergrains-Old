#include "FontResource.hpp"

#include <fstream>

#include "StreamStuff.hpp"

namespace pgg {

FontResource::FontResource()
: mLoaded(false) {
}

FontResource::~FontResource() {
}

bool FontResource::load() {
    if(mLoaded) {
        return true;
    }

    std::ifstream input(this->getFile().c_str(), std::ios::in | std::ios::binary);

    return true;
}
bool FontResource::unload() {
    mLoaded = false;
    return true;
}

}
