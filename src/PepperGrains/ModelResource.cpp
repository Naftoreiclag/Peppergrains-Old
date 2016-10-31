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

#include "ResourceManager.hpp"

namespace pgg {

ModelResource::ModelResource()
: mLoaded(false)
, mIsErrorResource(false)
, Resource(Resource::Type::MODEL) {
}

ModelResource::~ModelResource() {
}

void ModelResource::loadError() {
    assert(!mLoaded && "Attempted to load model that has already been loaded");
    
    ResourceManager* rmgr = ResourceManager::getSingleton();
    
    mGeometry = rmgr->getFallbackGeometry();
    mMaterial = rmgr->getFallbackMaterial();
    
    mGeometry->grab();
    mMaterial->grab();
    
    // Create a new vertex array object
    // This will be needed to quickly bind/unbind shader attributes and geometry buffers
    glGenVertexArrays(1, &mVertexArrayObject);
    glBindVertexArray(mVertexArrayObject);

    // Bind geometry buffers
    mGeometry->bindBuffers();

    // Tell OpenGL which attributes to read, how to read them, and where to send them
    const ShaderProgramResource* shaderProg = mMaterial->getShaderProg();
    if(shaderProg->needsPosAttrib()) {
        mGeometry->enablePositionAttrib(shaderProg->getPosAttrib());
    }
    if(shaderProg->needsColorAttrib()) {
        mGeometry->enableColorAttrib(shaderProg->getColorAttrib());
    }
    if(shaderProg->needsUVAttrib()) {
        mGeometry->enableUVAttrib(shaderProg->getUVAttrib());
    }
    if(shaderProg->needsNormalAttrib()) {
        mGeometry->enableNormalAttrib(shaderProg->getNormalAttrib());
    }
    if(shaderProg->needsTangentAttrib()) {
        mGeometry->enableTangentAttrib(shaderProg->getTangentAttrib());
    }
    if(shaderProg->needsBitangentAttrib()) {
        mGeometry->enableBitangentAttrib(shaderProg->getBitangentAttrib());
    }

    // Finished initalizing vertex array object, so unbind
    glBindVertexArray(0);
    
    mLoaded = true;
    mIsErrorResource = true;
}

void ModelResource::unloadError() {
    assert(mLoaded && "Attempted to unload model before loading it");
    
    mGeometry->drop();
    mMaterial->drop();

    // Tell OpenGL to free up the memory allocated during loading
    glDeleteVertexArrays(1, &mVertexArrayObject);
    
    mLoaded = false;
    mIsErrorResource = false;
}

void ModelResource::load() {
    assert(!mLoaded && "Attempted to load model that has already been loaded");
    
    if(this->isFallback()) {
        loadError();
        return;
    }

    ResourceManager* rmgr = ResourceManager::getSingleton();

    Json::Value mdlData;
    {
        std::ifstream loader(this->getFile().string().c_str());
        loader >> mdlData;
        loader.close();
    }

    const Json::Value& solidsData = mdlData["solids"];

    for(Json::Value::const_iterator iter = solidsData.begin(); iter != solidsData.end(); ++ iter) {
        const Json::Value& solidData = *iter;

        mGeometry = rmgr->findGeometry(solidData["geometry"].asString());
        mMaterial = rmgr->findMaterial(solidData["material"].asString());

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
    
    if(mIsErrorResource) {
        unloadError();
        return;
    }
    
    mGeometry->drop();
    mMaterial->drop();

    // Tell OpenGL to free up the memory allocated during loading
    glDeleteVertexArrays(1, &mVertexArrayObject);

    // Unloading complete
    mLoaded = false;
}

void ModelResource::render(const Renderable::Pass& rendPass, const glm::mat4& modelMat) {
    
    if(!mMaterial->isVisible(rendPass)) {
        return;
    }
    
    // Debug visibility check
    /*
    glm::vec4 ndc = rendPass.projMat * rendPass.viewMat * modelMat * glm::vec4(0.f, 0.f, 0.f, 1.f);
    ndc /= ndc.w;
    if(ndc.x < -1 || ndc.x > 1 || ndc.y < -1 || ndc.y > -1 || ndc.z < -1 || ndc.z > -1) {
        return;
    }
    
    if(rendPass.availableFustrumAABB) {
        glm::vec3 center = glm::vec3(modelMat * glm::vec4(0.f, 0.f, 0.f, 1.f));
        if(center.x > rendPass.maxBB.x || center.y > rendPass.maxBB.y || center.z > rendPass.maxBB.z || center.x < rendPass.minBB.x || center.y < rendPass.minBB.y || center.z < rendPass.minBB.z) {
            return;
        }
    }
    */
    
    mMaterial->use(rendPass, modelMat);

    // Bind the vertex array object from earlier (i.e. vertex attribute and geometry buffer info)
    glBindVertexArray(mVertexArrayObject);

    // Draw elements as specified by the geometry
    mGeometry->drawElements();

    // Unbind vertex array object
    glBindVertexArray(0);

    // Unbind shader program
    glUseProgram(0);
}

}
