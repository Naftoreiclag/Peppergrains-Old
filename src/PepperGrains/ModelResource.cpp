#include "ModelResource.hpp"

#include <fstream>

#include "json/json.h"

#include "ResourceManager.hpp"

ModelResource::ModelResource()
: mLoaded(false) {
}

ModelResource::~ModelResource() {
}

bool ModelResource::load() {
    if(mLoaded) {
        return true;
    }

    ResourceManager* rmgr = ResourceManager::getSingleton();

    Json::Value mdlData;

    {
        std::ifstream loader(this->getFile().c_str());
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

    mMaterial->grab();
    mGeometry->grab();

    glGenVertexArrays(1, &mVertexArrayObject);
    glBindVertexArray(mVertexArrayObject);

    mGeometry->bindBuffers();

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

    glBindVertexArray(0);

    mLoaded = true;
    return true;
}
bool ModelResource::unload() {
    mGeometry->drop();
    mMaterial->drop();

    glDeleteVertexArrays(1, &mVertexArrayObject);

    mLoaded = false;
    return false;
}

void ModelResource::render(const glm::mat4& viewMat, const glm::mat4& projMat, const glm::mat4& modelMat) {
    const ShaderProgramResource* shaderProg = mMaterial->getShaderProg();

    glUseProgram(shaderProg->getHandle());

    if(shaderProg->needsModelMatrix()) {
        glUniformMatrix4fv(shaderProg->getModelMatrixUnif(), 1, GL_FALSE, glm::value_ptr(modelMat));
    }
    if(shaderProg->needsViewMatrix()) {
        glUniformMatrix4fv(shaderProg->getViewMatrixUnif(), 1, GL_FALSE, glm::value_ptr(viewMat));
    }
    if(shaderProg->needsProjMatrix()) {
        glUniformMatrix4fv(shaderProg->getProjMatrixUnif(), 1, GL_FALSE, glm::value_ptr(projMat));
    }

    mMaterial->bindTextures();

    glBindVertexArray(mVertexArrayObject);
    mGeometry->render();
    glBindVertexArray(0);

    glUseProgram(0);
}
