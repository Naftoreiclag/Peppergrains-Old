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

#include "ModelInstance.hpp"

namespace pgg {

ModelInstance::ModelInstance(Model* model)
: mModel(model) {
    mModel->grab();
}

ModelInstance::ModelInstance()
: mModel(Model::getFallback()) {
    mModel->grab();
}

ModelInstance::ModelInstance(const ModelInstance& other)
: mModel(other.mModel)
, mModelMatr(other.mModelMatr) {
    mModel->grab();
}

ModelInstance& ModelInstance::operator=(const ModelInstance& other) {
    mModel->drop();
    mModel = other.mModel;
    mModel->grab();
    mModelMatr = other.mModelMatr;
}

ModelInstance::~ModelInstance() {
    mModel->drop();
}

Model* ModelInstance::getModel() const {
    return mModel;
}

}

