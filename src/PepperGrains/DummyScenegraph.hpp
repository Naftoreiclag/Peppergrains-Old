/*
   Copyright 2016 James Fong

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

#ifndef PGG_DUMMYSCENEGRAPH_HPP
#define PGG_DUMMYSCENEGRAPH_HPP

#include "Scenegraph.hpp"

namespace pgg {
class DummyScenegraph : public Scenegraph {
public:
    ModelInstance mModelInst;
    void render(std::function<void(ModelInstance)> modelMapper);
};

}

#endif // PGG_DUMMYSCENEGRAPH_HPP