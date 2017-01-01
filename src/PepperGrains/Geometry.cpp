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

#include "Geometry.hpp"

#include "GeometryResource.hpp"
#include "Logger.hpp"

namespace pgg {

Geometry* Geometry::getFallback() {
    return nullptr;
}
const std::vector<Geometry::Lightprobe>& Geometry::getLightprobes() const {
    static std::vector<Lightprobe> dummy;
    return dummy;
}
const Geometry::Armature& Geometry::getArmature() const {
    static Armature dummy;
    return dummy;
}


}

