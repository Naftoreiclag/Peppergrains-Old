#include "FontResource.hpp"

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

    return true;
}
bool FontResource::unload() {
    mLoaded = false;
    return true;
}

}
