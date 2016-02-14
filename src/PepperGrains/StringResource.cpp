#include "StringResource.hpp"

#include <fstream>
#include <iostream>
#include <sstream>

namespace pgg {

StringResource::StringResource()
: mLoaded(false) {
}

StringResource::~StringResource() {
}

bool StringResource::load() {
    if(mLoaded) {
        return true;
    }

    std::ifstream loader(this->getFile().c_str());
    std::stringstream ss;
    ss << loader.rdbuf();
    loader.close();

    mString = ss.str();
    mLoaded = true;
    return true;
}

bool StringResource::unload() {
    // Text files are not worth unloading, probably...

    return true;
}

const std::string& StringResource::getString() {
    return mString;
}

}
