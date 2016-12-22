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

#include "DummyRenderable.hpp"

#include <math.h>

namespace pgg {

DummyRenderable::DummyRenderable()
: mModel(nullptr)
, mModelMatr(glm::mat4(1.f))
, mPeriod(0.f) { }
DummyRenderable::~DummyRenderable() { }

void DummyRenderable::render(Renderable::Pass rendPass) {
    if(mModel) {
        mModel->render(rendPass, mModelMatr);
    }
}

void DummyRenderable::update(float tpf) {
    mPeriod += tpf;
    mModelMatr = glm::rotate(glm::mat4(1.f), std::sin(mPeriod), glm::vec3(0.f, 1.f, 0.f));
}

}

