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

#include "ShaderProgramResource.hpp"

#include <cassert>
#include <sstream>
#include <iostream>
#include <fstream>

#include "json/json.h"

#include "ResourceManager.hpp"

namespace pgg {

ShaderProgramResource::ShaderProgramResource()
: mLoaded(false)
, mIsErrorResource(false) {
}

ShaderProgramResource::~ShaderProgramResource() {
}

void ShaderProgramResource::loadError() {
    assert(!mLoaded && "Attempted to load shader program that has already been loaded");
    
    // Create program on GPU
    mShaderProg = glCreateProgram();

    // Generate shaders
    {
        // VERTEX
        
        const GLchar* vertSrc = 
            "#version 330                                                         \n"
            "in vec3 iPosition;                                                   \n"
            "in vec3 iNormal;                                                     \n"
            "in vec2 iUV;                                                         \n"
            "                                                                     \n"
            "out vec3 vNormal;                                                    \n"
            "out vec3 vPosition;                                                  \n"
            "out vec2 vUV;                                                        \n"
            "                                                                     \n"
            "uniform mat4 uModel;                                                 \n"
            "uniform mat4 uView;                                                  \n"
            "uniform mat4 uProj;                                                  \n"
            "                                                                     \n"
            "void main() {                                                        \n"
            "    gl_Position = uProj * uView * uModel * vec4(iPosition, 1.0);     \n"
            "    vUV = iUV;                                                       \n"
            "    vNormal = (uModel * vec4(iNormal, 0.0)).xyz;                     \n"
            "    vPosition = (uModel * vec4(iPosition, 1.0)).xyz;                 \n"
            "}                                                                    \n"
        ;

        mErrorVertShaderHandle = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(mErrorVertShaderHandle, 1, &vertSrc, 0);
        glCompileShader(mErrorVertShaderHandle);
        
        {
            GLint compileStatus;
            glGetShaderiv(mErrorVertShaderHandle, GL_COMPILE_STATUS, &compileStatus);
            if(compileStatus == GL_FALSE) {
                std::cout << "Error while compiling fallback/error vertex shader" << std::endl;
                char infoLog[512];
                glGetShaderInfoLog(mErrorVertShaderHandle, 512, 0, infoLog);
                std::cout << infoLog;
            }
        }
        
        // FRAGMENT
        
        const GLchar* fragSrc = 
            "#version 330                                      \n"
            "in vec3 vNormal;                                  \n"
            "in vec3 vPosition;                                \n"
            "in vec2 vUV;                                      \n"
            "                                                  \n"
            "uniform sampler2D ambientTex;                     \n"
            "                                                  \n"
            "out vec3 fColor;                                  \n"
            "out vec3 fNormal;                                 \n"
            "out vec3 fPosition;                               \n"
            "                                                  \n"
            "void main() {                                     \n"
            "    fColor = texture(ambientTex, vUV).rgb;        \n"
            "    fNormal = normalize(vNormal);                 \n"
            "    fPosition = vPosition;                        \n"
            "}                                                 \n"
        ;

        mErrorFragShaderHandle = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(mErrorFragShaderHandle, 1, &fragSrc, 0);
        glCompileShader(mErrorFragShaderHandle);
        
        {
            GLint compileStatus;
            glGetShaderiv(mErrorFragShaderHandle, GL_COMPILE_STATUS, &compileStatus);
            if(compileStatus == GL_FALSE) {
                std::cout << "Error while compiling fallback/error fragment shader" << std::endl;
                char infoLog[512];
                glGetShaderInfoLog(mErrorFragShaderHandle, 512, 0, infoLog);
                std::cout << infoLog;
            }
        }
    }
    
    // Attach all shaders
    glAttachShader(mShaderProg, mErrorVertShaderHandle);
    glAttachShader(mShaderProg, mErrorFragShaderHandle);
    
    // Setup fragment outputs
    glBindFragDataLocation(mShaderProg, 0, "fColor");
    glBindFragDataLocation(mShaderProg, 1, "fNormal");
    glBindFragDataLocation(mShaderProg, 2, "fPosition");
                
    // Link together shaders into a program
    glLinkProgram(mShaderProg);
    
    // Detach all shaders
    glDetachShader(mShaderProg, mErrorVertShaderHandle);
    glDetachShader(mShaderProg, mErrorFragShaderHandle);

    // Setup vertex attributes
    mUsePosAttrib = true;
    mUseColorAttrib = false;
    mUseUVAttrib = true;
    mUseNormalAttrib = true;
    mPosAttrib = glGetAttribLocation(mShaderProg, "iPosition");
    mUVAttrib = glGetAttribLocation(mShaderProg, "iUV");
    mNormalAttrib = glGetAttribLocation(mShaderProg, "iNormal");
                
    // Setup uniform matrices
    mUseModelMatrix = true;
    mUseViewMatrix = true;
    mUseProjMatrix = true;
    mModelMatrixUnif = glGetUniformLocation(mShaderProg, "uModel");
    mViewMatrixUnif = glGetUniformLocation(mShaderProg, "uView");
    mProjMatrixUnif = glGetUniformLocation(mShaderProg, "uProj");
        
    // Setup controls
    Control control;
    control.name = "Happy Birthday";
    control.handle = glGetUniformLocation(mShaderProg, "ambientTex");
    mUniformSampler2Ds.push_back(control);

    mLoaded = true;
    mIsErrorResource = true;
    
    std::cout << "Shader program error: " << this->getName() << std::endl;
}

void ShaderProgramResource::unloadError() {
    assert(mLoaded && "Attempted to unload shader program before loading it");
    
    glDeleteProgram(mShaderProg);
    
    glDeleteShader(mErrorVertShaderHandle);
    glDeleteShader(mErrorFragShaderHandle);
    
    mLoaded = false;
    mIsErrorResource = false;
    
}

void ShaderProgramResource::load() {
    assert(!mLoaded && "Attempted to load shader program that has already been loaded");

    if(this->isFallback()) {
        loadError();
        return;
    }

    // Load json data
    Json::Value progData;
    {
        std::ifstream loader(this->getFile().string().c_str());
        loader >> progData;
        loader.close();
    }

    // Get pointer to Resource Manager
    ResourceManager* rmgr = ResourceManager::getSingleton();

    // Grab shaders to link
    const Json::Value& links = progData["link"];
    for(Json::Value::const_iterator iter = links.begin(); iter != links.end(); ++ iter) {
        const Json::Value& value = *iter;
        std::string name = value.asString();
        ShaderResource* shader = rmgr->findShader(name);
        mLinkedShaders.push_back(shader);
        shader->grab();
    }

    // Create program on GPU
    mShaderProg = glCreateProgram();

    // Attach all shaders
    for(std::vector<ShaderResource*>::iterator iter = mLinkedShaders.begin(); iter != mLinkedShaders.end(); ++ iter) {
        ShaderResource* shader = *iter;
        glAttachShader(mShaderProg, shader->getHandle());
    }

    // Setup fragment outputs
    {
        const Json::Value& fragOut = progData["output"];
        
        if(fragOut.isNull()) {
        }
        else {
            const Json::Value& colorSym = fragOut["color"];
            const Json::Value& diffuseSym = fragOut["diffuse"];
            const Json::Value& normalSym = fragOut["normal"];
            //const Json::Value& brightSym = fragOut["bright"];
            
            // TODO: use enums instead of 0, 1, 2...
            if(!colorSym.isNull()) {
                std::string symbol = colorSym.asString();
                glBindFragDataLocation(mShaderProg, 0, symbol.c_str());
            }
            if(!diffuseSym.isNull()) {
                std::string symbol = diffuseSym.asString();
                glBindFragDataLocation(mShaderProg, 0, symbol.c_str());
            }
            if(!normalSym.isNull()) {
                std::string symbol = normalSym.asString();
                glBindFragDataLocation(mShaderProg, 1, symbol.c_str());
            }
        }
    }

    // Link together shaders into a program
    glLinkProgram(mShaderProg);

    // Detach all shaders
    for(std::vector<ShaderResource*>::iterator iter = mLinkedShaders.begin(); iter != mLinkedShaders.end(); ++ iter) {
        ShaderResource* shader = *iter;
        glDetachShader(mShaderProg, shader->getHandle());
    }

    // Setup vertex attributes
    {
        const Json::Value& attributes = progData["attributes"];
        if(attributes.isNull()) {
            mUsePosAttrib = false;
            mUseColorAttrib = false;
            mUseUVAttrib = false;
            mUseNormalAttrib = false;
        }
        else {
            const Json::Value& posSym = attributes["position"];
            const Json::Value& diffuseSym = attributes["color"];
            const Json::Value& uvSym = attributes["uv"];
            const Json::Value& normalSym = attributes["normal"];
            const Json::Value& tangentSym = attributes["tangent"];
            const Json::Value& bitangentSym = attributes["bitangent"];

            if(posSym.isNull()) {
                mUsePosAttrib = false;
            } else {
                mUsePosAttrib = true;
                std::string symbol = posSym.asString();
                mPosAttrib = glGetAttribLocation(mShaderProg, symbol.c_str());
            }
            if(diffuseSym.isNull()) {
                mUseColorAttrib = false;
            } else {
                mUseColorAttrib = true;
                std::string symbol = diffuseSym.asString();
                mColorAttrib = glGetAttribLocation(mShaderProg, symbol.c_str());
            }
            if(uvSym.isNull()) {
                mUseUVAttrib = false;
            } else {
                mUseUVAttrib = true;
                std::string symbol = uvSym.asString();
                mUVAttrib = glGetAttribLocation(mShaderProg, symbol.c_str());
            }
            if(normalSym.isNull()) {
                mUseNormalAttrib = false;
            } else {
                mUseNormalAttrib = true;
                std::string symbol = normalSym.asString();
                mNormalAttrib = glGetAttribLocation(mShaderProg, symbol.c_str());
            }
            if(tangentSym.isNull()) {
                mUseTangentAttrib = false;
            } else {
                mUseTangentAttrib = true;
                std::string symbol = tangentSym.asString();
                mTangentAttrib = glGetAttribLocation(mShaderProg, symbol.c_str());
            }
            if(bitangentSym.isNull()) {
                mUseBitangentAttrib = false;
            } else {
                mUseBitangentAttrib = true;
                std::string symbol = bitangentSym.asString();
                mBitangentAttrib = glGetAttribLocation(mShaderProg, symbol.c_str());
            }
        }
    }

    // Setup uniform matrices
    {
        const Json::Value& matrices = progData["matrices"];
        if(matrices.isNull()) {
            mUseModelMatrix = false;
            mUseViewMatrix = false;
            mUseProjMatrix = false;
        }
        else {
            const Json::Value& mMat = matrices["model"];
            const Json::Value& vMat = matrices["view"];
            const Json::Value& pMat = matrices["projection"];
            const Json::Value& mvMat = matrices["modelView"];
            const Json::Value& vpMat = matrices["viewProjection"];
            const Json::Value& mvpMat = matrices["modelViewProjection"];
            const Json::Value& imMat = matrices["inverseModel"];
            const Json::Value& ivMat = matrices["inverseView"];
            const Json::Value& ipMat = matrices["inverseProjection"];
            const Json::Value& imvMat = matrices["inverseModelView"];
            const Json::Value& ivpMat = matrices["inverseViewProjection"];
            const Json::Value& imvpMat = matrices["inverseModelViewProjection"];
            const Json::Value& svpMat = matrices["sunViewProjection"];

            if(mMat.isNull()) {
                mUseModelMatrix = false;
            } else {
                mUseModelMatrix = true;
                std::string symbol = mMat.asString();
                mModelMatrixUnif = glGetUniformLocation(mShaderProg, symbol.c_str());
            }
            if(vMat.isNull()) {
                mUseViewMatrix = false;
            } else {
                mUseViewMatrix = true;
                std::string symbol = vMat.asString();
                mViewMatrixUnif = glGetUniformLocation(mShaderProg, symbol.c_str());
            }
            if(pMat.isNull()) {
                mUseProjMatrix = false;
            } else {
                mUseProjMatrix = true;
                std::string symbol = pMat.asString();
                mProjMatrixUnif = glGetUniformLocation(mShaderProg, symbol.c_str());
            }
            if(ivpMat.isNull()) {
                mUseInvViewProjMatrix = false;
            } else {
                mUseInvViewProjMatrix = true;
                std::string symbol = ivpMat.asString();
                mInvViewProjMatrixUnif = glGetUniformLocation(mShaderProg, symbol.c_str());
            }
            if(svpMat.isNull()) {
                mUseSunViewProjMatrix = false;
            } else {
                mUseSunViewProjMatrix = true;
                std::string symbol = svpMat.asString();
                mSunViewProjMatrixUnif = glGetUniformLocation(mShaderProg, symbol.c_str());
            }
        }
    }

    // Setup instancing data
    {
        const Json::Value& instancing = progData["instancing"];
        
        if(!instancing.isNull()) {
            const Json::Value& sampler2Ds = instancing["sampler2D"];
            const Json::Value& vec2s = instancing["vec2"];
            const Json::Value& vec3s = instancing["vec3"];
            const Json::Value& vec4s = instancing["vec4"];

            if(!sampler2Ds.isNull()) {
                for(Json::Value::const_iterator iter = sampler2Ds.begin(); iter != sampler2Ds.end(); ++ iter) {
                    const Json::Value& key = iter.key();
                    const Json::Value& value = *iter;

                    // Key = name used for referencing
                    // Value = symbol searched for in shader code

                    Control control;
                    control.name = key.asString();
                    control.handle = glGetAttribLocation(mShaderProg, value.asString().c_str());

                    mInstancedSampler2Ds.push_back(control);
                }
            }
            if(!vec2s.isNull()) {
                for(Json::Value::const_iterator iter = vec2s.begin(); iter != vec2s.end(); ++ iter) {
                    const Json::Value& key = iter.key();
                    const Json::Value& value = *iter;

                    Control control;
                    control.name = key.asString();
                    control.handle = glGetAttribLocation(mShaderProg, value.asString().c_str());

                    mInstancedVec2s.push_back(control);
                }
            }
            if(!vec3s.isNull()) {
                for(Json::Value::const_iterator iter = vec3s.begin(); iter != vec3s.end(); ++ iter) {
                    const Json::Value& key = iter.key();
                    const Json::Value& value = *iter;

                    Control control;
                    control.name = key.asString();
                    control.handle = glGetAttribLocation(mShaderProg, value.asString().c_str());

                    mInstancedVec3s.push_back(control);
                }
            }
            if(!vec4s.isNull()) {
                for(Json::Value::const_iterator iter = vec4s.begin(); iter != vec4s.end(); ++ iter) {
                    const Json::Value& key = iter.key();
                    const Json::Value& value = *iter;

                    Control control;
                    control.name = key.asString();
                    control.handle = glGetAttribLocation(mShaderProg, value.asString().c_str());

                    mInstancedVec4s.push_back(control);
                }
            }
        }
    }

    // Setup controls
    {
        const Json::Value& instancing = progData["controls"];

        if(!instancing.isNull()) {
            const Json::Value& sampler2Ds = instancing["sampler2D"];
            const Json::Value& vec2s = instancing["vec2"];
            const Json::Value& vec3s = instancing["vec3"];
            const Json::Value& vec4s = instancing["vec4"];

            if(!sampler2Ds.isNull()) {
                for(Json::Value::const_iterator iter = sampler2Ds.begin(); iter != sampler2Ds.end(); ++ iter) {
                    const Json::Value& key = iter.key();
                    const Json::Value& value = *iter;

                    // Key = name used for referencing
                    // Value = symbol searched for in shader code

                    Control control;
                    control.name = key.asString();
                    control.handle = glGetUniformLocation(mShaderProg, value.asString().c_str());

                    mUniformSampler2Ds.push_back(control);
                }
            }
            if(!vec2s.isNull()) {
                for(Json::Value::const_iterator iter = vec2s.begin(); iter != vec2s.end(); ++ iter) {
                    const Json::Value& key = iter.key();
                    const Json::Value& value = *iter;

                    Control control;
                    control.name = key.asString();
                    control.handle = glGetUniformLocation(mShaderProg, value.asString().c_str());

                    mUniformVec2s.push_back(control);
                }
            }
            if(!vec3s.isNull()) {
                for(Json::Value::const_iterator iter = vec3s.begin(); iter != vec3s.end(); ++ iter) {
                    const Json::Value& key = iter.key();
                    const Json::Value& value = *iter;

                    Control control;
                    control.name = key.asString();
                    control.handle = glGetUniformLocation(mShaderProg, value.asString().c_str());

                    mUniformVec3s.push_back(control);
                }
            }
            if(!vec4s.isNull()) {
                for(Json::Value::const_iterator iter = vec4s.begin(); iter != vec4s.end(); ++ iter) {
                    const Json::Value& key = iter.key();
                    const Json::Value& value = *iter;

                    Control control;
                    control.name = key.asString();
                    control.handle = glGetUniformLocation(mShaderProg, value.asString().c_str());

                    mUniformVec4s.push_back(control);
                }
            }
        }
    }

    mLoaded = true;
}

void ShaderProgramResource::unload() {
    assert(mLoaded && "Attempted to unload shader program before loading it");
    
    if(mIsErrorResource) {
        unloadError();
        return;
    }
    
    // Free OpenGL shader program
    glDeleteProgram(mShaderProg);

    // Drop all grabbed resources
    for(std::vector<ShaderResource*>::iterator iter = mLinkedShaders.begin(); iter != mLinkedShaders.end(); ++ iter) {
        ShaderResource* shader = *iter;
        shader->drop();
    }

    mLoaded = false;
}

void ShaderProgramResource::bindModelViewProjMatrices(const glm::mat4& modelMat, const glm::mat4& viewMat, const glm::mat4& projMat) const {
    if(this->needsModelMatrix()) {
        glUniformMatrix4fv(this->getModelMatrixUnif(), 1, GL_FALSE, glm::value_ptr(modelMat));
    }
    if(this->needsViewMatrix()) {
        glUniformMatrix4fv(this->getViewMatrixUnif(), 1, GL_FALSE, glm::value_ptr(viewMat));
    }
    if(this->needsProjMatrix()) {
        glUniformMatrix4fv(this->getProjMatrixUnif(), 1, GL_FALSE, glm::value_ptr(projMat));
    }
    if(this->needsInvViewProjMatrix()) {
        glm::mat4 invViewProjMat = glm::inverse(projMat * viewMat);
        glUniformMatrix4fv(this->getInvViewProjMatrixUnif(), 1, GL_FALSE, glm::value_ptr(invViewProjMat));
    }
}

GLuint ShaderProgramResource::getHandle() const { return mShaderProg; }
bool ShaderProgramResource::needsModelMatrix() const { return mUseModelMatrix; }
bool ShaderProgramResource::needsViewMatrix() const { return mUseViewMatrix; }
bool ShaderProgramResource::needsProjMatrix() const { return mUseProjMatrix; }
bool ShaderProgramResource::needsInvViewProjMatrix() const { return mUseInvViewProjMatrix; }
bool ShaderProgramResource::needsSunViewProjMatrix() const { return mUseSunViewProjMatrix; }
bool ShaderProgramResource::needsPosAttrib() const { return mUsePosAttrib; }
bool ShaderProgramResource::needsColorAttrib() const { return mUseColorAttrib; }
bool ShaderProgramResource::needsUVAttrib() const { return mUseUVAttrib; }
bool ShaderProgramResource::needsNormalAttrib() const { return mUseNormalAttrib; }
bool ShaderProgramResource::needsTangentAttrib() const { return mUseTangentAttrib; }
bool ShaderProgramResource::needsBitangentAttrib() const { return mUseBitangentAttrib; }
GLuint ShaderProgramResource::getModelMatrixUnif() const { return mModelMatrixUnif; }
GLuint ShaderProgramResource::getViewMatrixUnif() const { return mViewMatrixUnif; }
GLuint ShaderProgramResource::getProjMatrixUnif() const { return mProjMatrixUnif; }
GLuint ShaderProgramResource::getInvViewProjMatrixUnif() const { return mInvViewProjMatrixUnif; }
GLuint ShaderProgramResource::getSunViewProjMatrixUnif() const { return mSunViewProjMatrixUnif; }
GLuint ShaderProgramResource::getPosAttrib() const { return mPosAttrib; }
GLuint ShaderProgramResource::getColorAttrib() const { return mColorAttrib; }
GLuint ShaderProgramResource::getUVAttrib() const { return mUVAttrib; }
GLuint ShaderProgramResource::getNormalAttrib() const { return mNormalAttrib; }
GLuint ShaderProgramResource::getTangentAttrib() const { return mTangentAttrib; }
GLuint ShaderProgramResource::getBitangentAttrib() const { return mBitangentAttrib; }
const std::vector<ShaderProgramResource::Control>& ShaderProgramResource::getUniformSampler2Ds() const { return mUniformSampler2Ds; }
const std::vector<ShaderProgramResource::Control>& ShaderProgramResource::getUniformVec2s() const { return mUniformVec2s; }
const std::vector<ShaderProgramResource::Control>& ShaderProgramResource::getUniformVec3s() const { return mUniformVec3s; }
const std::vector<ShaderProgramResource::Control>& ShaderProgramResource::getUniformVec4s() const { return mUniformVec4s; }
const std::vector<ShaderProgramResource::Control>& ShaderProgramResource::getInstancedSampler2Ds() const { return mInstancedSampler2Ds; }
const std::vector<ShaderProgramResource::Control>& ShaderProgramResource::getInstancedVec2s() const { return mInstancedVec2s; }
const std::vector<ShaderProgramResource::Control>& ShaderProgramResource::getInstancedVec3s() const { return mInstancedVec3s; }
const std::vector<ShaderProgramResource::Control>& ShaderProgramResource::getInstancedVec4s() const { return mInstancedVec4s; }

}
