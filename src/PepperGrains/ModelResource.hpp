/*
   Copyright 2015-2016 James Fong

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

#ifndef PGG_MODELRESOURCE_HPP
#define PGG_MODELRESOURCE_HPP

#include "GeometryResource.hpp"
#include "MaterialResource.hpp"
#include "Model.hpp"
#include "Resource.hpp"

namespace pgg {

class ModelResource : public Resource, public Model {
private:
    bool mLoaded;

    GeometryResource* mGeometry;
    MaterialResource* mMaterial;
    GLuint mVertexArrayObject;
    
    void loadError();
    void unloadError();
    bool mIsErrorResource;
public:
    ModelResource();
    virtual ~ModelResource();
    
    static ModelResource* upcast(Resource* resource);
    static ModelResource* getFallback();
public:
    void load();
    void unload();

    // Render this model with the provided matrices
    void render(Renderable::Pass rendPass, const glm::mat4& modelMat);
};

}

#endif // MODELRESOURCE_HPP
