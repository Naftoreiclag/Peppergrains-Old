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
    Sampler2DControl control;
    control.name = "Happy Birthday";
    control.handle = glGetUniformLocation(mShaderProg, "ambientTex");
    mSampler2Ds.push_back(control);

    mLoaded = true;
    mIsErrorResource = true;
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
            const Json::Value& posSym = fragOut["position"];
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
            if(!posSym.isNull()) {
                std::string symbol = posSym.asString();
                glBindFragDataLocation(mShaderProg, 2, symbol.c_str());
            }
            /*
            if(!brightSym.isNull()) {
                std::string symbol = brightSym.asString();
                glBindFragDataLocation(mShaderProg, 3, symbol.c_str());
            }
            */
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
            const Json::Value& modelMatrix = matrices["model"];
            const Json::Value& viewMatrix = matrices["view"];
            const Json::Value& projMatrix = matrices["projection"];
            const Json::Value& invViewProjMatrix = matrices["inverseViewProjection"];

            if(modelMatrix.isNull()) {
                mUseModelMatrix = false;
            } else {
                mUseModelMatrix = true;
                std::string symbol = modelMatrix.asString();
                mModelMatrixUnif = glGetUniformLocation(mShaderProg, symbol.c_str());
            }
            if(viewMatrix.isNull()) {
                mUseViewMatrix = false;
            } else {
                mUseViewMatrix = true;
                std::string symbol = viewMatrix.asString();
                mViewMatrixUnif = glGetUniformLocation(mShaderProg, symbol.c_str());
            }
            if(projMatrix.isNull()) {
                mUseProjMatrix = false;
            } else {
                mUseProjMatrix = true;
                std::string symbol = projMatrix.asString();
                mProjMatrixUnif = glGetUniformLocation(mShaderProg, symbol.c_str());
            }
            if(invViewProjMatrix.isNull()) {
                mUseInvViewProjMatrix = false;
            } else {
                mUseInvViewProjMatrix = true;
                std::string symbol = invViewProjMatrix.asString();
                mInvViewProjMatrixUnif = glGetUniformLocation(mShaderProg, symbol.c_str());
            }
        }
    }

    // Setup controls
    {
        const Json::Value& controls = progData["controls"];

        if(!controls.isNull()) {
            const Json::Value& sampler2Ds = controls["sampler2D"];

            if(!sampler2Ds.isNull()) {
                for(Json::Value::const_iterator iter = sampler2Ds.begin(); iter != sampler2Ds.end(); ++ iter) {
                    const Json::Value& key = iter.key();
                    const Json::Value& value = *iter;

                    // Key = name used for referencing
                    // Value = symbol searched for in shader code

                    Sampler2DControl control;
                    control.name = key.asString();
                    control.handle = glGetUniformLocation(mShaderProg, value.asString().c_str());

                    mSampler2Ds.push_back(control);
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

GLuint ShaderProgramResource::getHandle() const { return mShaderProg; }
bool ShaderProgramResource::needsModelMatrix() const { return mUseModelMatrix; }
bool ShaderProgramResource::needsViewMatrix() const { return mUseViewMatrix; }
bool ShaderProgramResource::needsProjMatrix() const { return mUseProjMatrix; }
bool ShaderProgramResource::needsInvViewProjMatrix() const { return mUseInvViewProjMatrix; }
bool ShaderProgramResource::needsPosAttrib() const { return mUsePosAttrib; }
bool ShaderProgramResource::needsColorAttrib() const { return mUseColorAttrib; }
bool ShaderProgramResource::needsUVAttrib() const { return mUseUVAttrib; }
bool ShaderProgramResource::needsNormalAttrib() const { return mUseNormalAttrib; }
GLuint ShaderProgramResource::getModelMatrixUnif() const { return mModelMatrixUnif; }
GLuint ShaderProgramResource::getViewMatrixUnif() const { return mViewMatrixUnif; }
GLuint ShaderProgramResource::getProjMatrixUnif() const { return mProjMatrixUnif; }
GLuint ShaderProgramResource::getInvViewProjMatrixUnif() const { return mInvViewProjMatrixUnif; }
GLuint ShaderProgramResource::getPosAttrib() const { return mPosAttrib; }
GLuint ShaderProgramResource::getColorAttrib() const { return mColorAttrib; }
GLuint ShaderProgramResource::getUVAttrib() const { return mUVAttrib; }
GLuint ShaderProgramResource::getNormalAttrib() const { return mNormalAttrib; }
const std::vector<ShaderProgramResource::Sampler2DControl>& ShaderProgramResource::getSampler2Ds() const { return mSampler2Ds; }

}
