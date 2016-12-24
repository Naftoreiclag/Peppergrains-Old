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

#include "ModelResource.hpp"

#include <cassert>
#include <iostream>
#include <fstream>

#include "json/json.h"

#include "Logger.hpp"
#include "Resources.hpp"

#include "MaterialResource.hpp"
#include "GeometryResource.hpp"

namespace pgg {

ModelResource::ModelResource()
: mLoaded(false)
, mIsErrorResource(false)
, Resource(Resource::Type::MODEL) {
}

ModelResource::~ModelResource() {
}

Model* ModelResource::gallop(Resource* resource) {
    if(!resource || resource->mResourceType != Resource::Type::MODEL) {
        Logger::log(Logger::WARN) << "Failed to cast " << (resource ? resource->getName() : "nullptr") << " to model!" << std::endl;
        return Model::getFallback();
    } else {
        return static_cast<ModelResource*>(resource);
    }
}

void ModelResource::load() {
    assert(!mLoaded && "Attempted to load model that has already been loaded");

    Json::Value mdlData;
    {
        std::ifstream loader(this->getFile().string().c_str());
        loader >> mdlData;
        loader.close();
    }

    const Json::Value& solidsData = mdlData["solids"];

    for(Json::Value::const_iterator iter = solidsData.begin(); iter != solidsData.end(); ++ iter) {
        const Json::Value& solidData = *iter;

        mGeometry = GeometryResource::gallop(Resources::find(solidData["geometry"].asString()));
        mMaterial = MaterialResource::gallop(Resources::find(solidData["material"].asString()));

        break;
    }

    mGeometry->grab();
    mMaterial->grab();

    // Create a new vertex array object
    // This will be needed to quickly bind/unbind shader attributes and geometry buffers
    glGenVertexArrays(1, &mVertexArrayObject);
    glBindVertexArray(mVertexArrayObject);

    // Bind geometry buffers
    mGeometry->bindBuffers();

    // Tell OpenGL which attributes to read, how to read them, and where to send them
    mMaterial->enableVertexAttributesFor(mGeometry);

    // Finished initalizing vertex array object, so unbind
    glBindVertexArray(0);

    // Loading complete
    mLoaded = true;
}
void ModelResource::unload() {
    assert(mLoaded && "Attempted to unload model before loading it");
    
    mGeometry->drop();
    mMaterial->drop();

    // Tell OpenGL to free up the memory allocated during loading
    glDeleteVertexArrays(1, &mVertexArrayObject);

    // Unloading complete
    mLoaded = false;
}

void ModelResource::render(Renderable::Pass rendPass, const glm::mat4& modelMat) {
    
    if(!mMaterial->isVisible(rendPass)) {
        return;
    }
    
    mMaterial->useProgram(rendPass, modelMat);

    // Bind the vertex array object from earlier (i.e. vertex attribute and geometry buffer info)
    glBindVertexArray(mVertexArrayObject);

    // Draw elements as specified by the geometry
    mGeometry->drawElements();

    // Unbind vertex array object
    glBindVertexArray(0);

    // Unbind shader program
    glUseProgram(0);
}
Geometry* ModelResource::getGeometry() const {
    return mGeometry;
}
Material* ModelResource::getMaterial() const {
    return mMaterial;
}

void ModelResource::bindVertexArray() {
    glBindVertexArray(mVertexArrayObject);
}

}
