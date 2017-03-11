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

#include "Model.hpp"

#include "Logger.hpp"

namespace pgg {

Model* Model::getFallback() {
    static Model fallbackModel;
    return &fallbackModel;
}

Model::Model()
: mGeometry(Geometry::getFallback())
, mMaterial(Material::getFallback()) {
}

Model::Model(Geometry* geometry, Material* material)
: mGeometry(geometry)
, mMaterial(material) {
}

Model::~Model() { }

void Model::load() {
    mGeometry->grab();
    mMaterial->grab();
}
void Model::unload() {
    mGeometry->drop();
    mMaterial->drop();
    
    delete this;
}

Geometry* Model::getGeometry() const { return mGeometry; }
Material* Model::getMaterial() const { return mMaterial; }

#ifdef PGG_OPENGL
void Model::bindVertexArray() { }
#endif

}
