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

#include "Logger.hpp"
#include "Resources.hpp"

namespace pgg {

ShaderProgramResource::ShaderProgramResource()
: mLoaded(false)
, mIsErrorResource(false)
, Resource(Resource::Type::SHADER_PROGRAM) {
}

ShaderProgramResource::~ShaderProgramResource() {
}

ShaderProgramResource* ShaderProgramResource::upcast(Resource* resource) {
    if(!resource || resource->mResourceType != Resource::Type::SHADER_PROGRAM) {
        Logger::log(Logger::WARN) << "Failed to cast " << resource->getName() << " to shader program!" << std::endl;
        return getFallback();
    } else {
        return static_cast<ShaderProgramResource*>(resource);
    }
}
ShaderProgramResource* ShaderProgramResource::getFallback() {
    return nullptr;
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
    mUseMMat = true;
    mUseVMat = true;
    mUsePMat = true;
    mMMatUnif = glGetUniformLocation(mShaderProg, "uModel");
    mVMatUnif = glGetUniformLocation(mShaderProg, "uView");
    mPMatUnif = glGetUniformLocation(mShaderProg, "uProj");
    
    mUseMVMat = false;
    mUseVPMat = false;
    mUseMVPMat = false;
    mUseIMMat = false;
    mUseIVMat = false;
    mUseIPMat = false;
    mUseIMVMat = false;
    mUseIVPMat = false;
    mUseIMVPMat = false;
        
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

    // Grab shaders to link
    const Json::Value& links = progData["link"];
    for(Json::Value::const_iterator iter = links.begin(); iter != links.end(); ++ iter) {
        const Json::Value& value = *iter;
        std::string name = value.asString();
        ShaderResource* shader = ShaderResource::upcast(Resources::find(name));
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
            const Json::Value& brightSym = fragOut["bright"];
            const Json::Value& ssipgDepthSym = fragOut["ssipg-depth"];
            const Json::Value& ssipgDiffuseSym = fragOut["ssipg-diffuse"];
            const Json::Value& ssipgOrientSym = fragOut["ssipg-orientation"];
            const Json::Value& ssipgForceSym = fragOut["ssipg-force"];
            
            // TODO: use enums instead of 0, 1, 2...
            if(!colorSym.isNull()) {
                glBindFragDataLocation(mShaderProg, 0, colorSym.asString().c_str());
            }
            if(!diffuseSym.isNull()) {
                glBindFragDataLocation(mShaderProg, 0, diffuseSym.asString().c_str());
            }
            if(!normalSym.isNull()) {
                glBindFragDataLocation(mShaderProg, 1, normalSym.asString().c_str());
            }
            if(!brightSym.isNull()) {
                glBindFragDataLocation(mShaderProg, 0, brightSym.asString().c_str());
            }
            if(!ssipgDiffuseSym.isNull()) {
                glBindFragDataLocation(mShaderProg, 0, ssipgDiffuseSym.asString().c_str());
            }
            if(!ssipgDepthSym.isNull()) {
                glBindFragDataLocation(mShaderProg, 1, ssipgDepthSym.asString().c_str());
            }
            if(!ssipgOrientSym.isNull()) {
                glBindFragDataLocation(mShaderProg, 2, ssipgOrientSym.asString().c_str());
            }
            if(!ssipgForceSym.isNull()) {
                glBindFragDataLocation(mShaderProg, 3, ssipgForceSym.asString().c_str());
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
        const Json::Value& passUniforms = progData["pass-uniforms"];
        if(passUniforms.isNull()) {
            mUseMMat = false;
            mUseVMat = false;
            mUsePMat = false;
        }
        else {
            const Json::Value& mMat = passUniforms["model"];
            const Json::Value& vMat = passUniforms["view"];
            const Json::Value& pMat = passUniforms["projection"];
            const Json::Value& mvMat = passUniforms["modelView"];
            const Json::Value& vpMat = passUniforms["viewProjection"];
            const Json::Value& mvpMat = passUniforms["modelViewProjection"];
            const Json::Value& imMat = passUniforms["inverseModel"];
            const Json::Value& ivMat = passUniforms["inverseView"];
            const Json::Value& ipMat = passUniforms["inverseProjection"];
            const Json::Value& imvMat = passUniforms["inverseModelView"];
            const Json::Value& ivpMat = passUniforms["inverseViewProjection"];
            const Json::Value& imvpMat = passUniforms["inverseModelViewProjection"];
            const Json::Value& svpMat = passUniforms["sunViewProjection"];
            const Json::Value& scrSz = passUniforms["screenSize"];
            const Json::Value& iScrSz = passUniforms["inverseScreenSize"];
            const Json::Value& camLoc = passUniforms["cameraLocation"];
            const Json::Value& camDir = passUniforms["cameraDirection"];

            if(mMat.isNull()) { mUseMMat = false; } else { mUseMMat = true;
                mMMatUnif = glGetUniformLocation(mShaderProg, mMat.asString().c_str());
            }
            if(vMat.isNull()) { mUseVMat = false; } else { mUseVMat = true;
                mVMatUnif = glGetUniformLocation(mShaderProg, vMat.asString().c_str());
            }
            if(pMat.isNull()) { mUsePMat = false; } else { mUsePMat = true;
                mPMatUnif = glGetUniformLocation(mShaderProg, pMat.asString().c_str());
            }
            if(mvMat.isNull()) { mUseMVMat = false; } else { mUseMVMat = true;
                mMVMatUnif = glGetUniformLocation(mShaderProg, mvMat.asString().c_str());
            }
            if(vpMat.isNull()) { mUseVPMat = false; } else { mUseVPMat = true;
                mVPMatUnif = glGetUniformLocation(mShaderProg, vpMat.asString().c_str());
            }
            if(mvpMat.isNull()) { mUseMVPMat = false; } else { mUseMVPMat = true;
                mMVPMatUnif = glGetUniformLocation(mShaderProg, mvpMat.asString().c_str());
            }
            if(imMat.isNull()) { mUseIMMat = false; } else { mUseIMMat = true;
                mIMMatUnif = glGetUniformLocation(mShaderProg, imMat.asString().c_str());
            }
            if(ivMat.isNull()) { mUseIVMat = false; } else { mUseIVMat = true;
                mIVMatUnif = glGetUniformLocation(mShaderProg, ivMat.asString().c_str());
            }
            if(ipMat.isNull()) { mUseIPMat = false; } else { mUseIPMat = true;
                mIPMatUnif = glGetUniformLocation(mShaderProg, ipMat.asString().c_str());
            }
            if(imvMat.isNull()) { mUseIMVMat = false; } else { mUseIMVMat = true;
                mIMVMatUnif = glGetUniformLocation(mShaderProg, imvMat.asString().c_str());
            }
            if(ivpMat.isNull()) { mUseIVPMat = false; } else { mUseIVPMat = true;
                mIVPMatUnif = glGetUniformLocation(mShaderProg, ivpMat.asString().c_str());
            }
            if(imvpMat.isNull()) { mUseIMVPMat = false; } else { mUseIMVPMat = true;
                mIMVPMatUnif = glGetUniformLocation(mShaderProg, imvpMat.asString().c_str());
            }
            if(svpMat.isNull()) { mUseSunViewProjMatrix = false; } else { mUseSunViewProjMatrix = true;
                mSunViewProjMatrixUnif = glGetUniformLocation(mShaderProg, svpMat.asString().c_str());
            }
            if(scrSz.isNull()) { mUseScreenSize = false; } else { mUseScreenSize = true;
                mScreenSizeUnif = glGetUniformLocation(mShaderProg, scrSz.asString().c_str());
            }
            if(iScrSz.isNull()) { mUseIScreenSize = false; } else { mUseIScreenSize = true;
                mIScreenSizeUnif = glGetUniformLocation(mShaderProg, iScrSz.asString().c_str());
            }
            if(camLoc.isNull()) { mUseCameraLoc = false; } else { mUseCameraLoc = true;
                mCameraLocUnif = glGetUniformLocation(mShaderProg, camLoc.asString().c_str());
            }
            if(camDir.isNull()) { mUseCameraDir = false; } else { mUseCameraDir = true;
                mCameraDirUnif = glGetUniformLocation(mShaderProg, camDir.asString().c_str());
            }
        }
    }

    // Setup instancing data
    {
        const Json::Value& instancing = progData["instancing"];
        
        if(!instancing.isNull()) {
            const Json::Value& sampler2Ds = instancing["sampler2D"];
            const Json::Value& floats = instancing["float"];
            const Json::Value& ints = instancing["int"];
            const Json::Value& uints = instancing["uint"];
            const Json::Value& vec2s = instancing["vec2"];
            const Json::Value& vec3s = instancing["vec3"];
            const Json::Value& vec4s = instancing["vec4"];
            const Json::Value& mat4s = instancing["mat4"];

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
            if(!floats.isNull()) {
                for(Json::Value::const_iterator iter = floats.begin(); iter != floats.end(); ++ iter) {
                    const Json::Value& key = iter.key();
                    const Json::Value& value = *iter;

                    Control control;
                    control.name = key.asString();
                    control.handle = glGetAttribLocation(mShaderProg, value.asString().c_str());

                    mInstancedFloats.push_back(control);
                }
            }
            if(!ints.isNull()) {
                for(Json::Value::const_iterator iter = ints.begin(); iter != ints.end(); ++ iter) {
                    const Json::Value& key = iter.key();
                    const Json::Value& value = *iter;

                    Control control;
                    control.name = key.asString();
                    control.handle = glGetAttribLocation(mShaderProg, value.asString().c_str());

                    mInstancedInts.push_back(control);
                }
            }
            if(!uints.isNull()) {
                for(Json::Value::const_iterator iter = uints.begin(); iter != uints.end(); ++ iter) {
                    const Json::Value& key = iter.key();
                    const Json::Value& value = *iter;

                    Control control;
                    control.name = key.asString();
                    control.handle = glGetAttribLocation(mShaderProg, value.asString().c_str());

                    mInstancedUints.push_back(control);
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
            if(!mat4s.isNull()) {
                for(Json::Value::const_iterator iter = mat4s.begin(); iter != mat4s.end(); ++ iter) {
                    const Json::Value& key = iter.key();
                    const Json::Value& value = *iter;

                    Control control;
                    control.name = key.asString();
                    control.handle = glGetAttribLocation(mShaderProg, value.asString().c_str());

                    mInstancedMat4s.push_back(control);
                }
            }
        }
    }

    // Setup controls
    {
        const Json::Value& instancing = progData["controls"];

        if(!instancing.isNull()) {
            const Json::Value& sampler2Ds = instancing["sampler2D"];
            const Json::Value& floats = instancing["float"];
            const Json::Value& ints = instancing["int"];
            const Json::Value& uints = instancing["uint"];
            const Json::Value& vec2s = instancing["vec2"];
            const Json::Value& vec3s = instancing["vec3"];
            const Json::Value& vec4s = instancing["vec4"];
            const Json::Value& mat4s = instancing["mat4"];

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
            if(!floats.isNull()) {
                for(Json::Value::const_iterator iter = floats.begin(); iter != floats.end(); ++ iter) {
                    const Json::Value& key = iter.key();
                    const Json::Value& value = *iter;

                    Control control;
                    control.name = key.asString();
                    control.handle = glGetUniformLocation(mShaderProg, value.asString().c_str());

                    mUniformFloats.push_back(control);
                }
            }
            if(!ints.isNull()) {
                for(Json::Value::const_iterator iter = ints.begin(); iter != ints.end(); ++ iter) {
                    const Json::Value& key = iter.key();
                    const Json::Value& value = *iter;

                    Control control;
                    control.name = key.asString();
                    control.handle = glGetUniformLocation(mShaderProg, value.asString().c_str());

                    mUniformInts.push_back(control);
                }
            }
            if(!uints.isNull()) {
                for(Json::Value::const_iterator iter = uints.begin(); iter != uints.end(); ++ iter) {
                    const Json::Value& key = iter.key();
                    const Json::Value& value = *iter;

                    Control control;
                    control.name = key.asString();
                    control.handle = glGetUniformLocation(mShaderProg, value.asString().c_str());

                    mUniformUints.push_back(control);
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
            if(!mat4s.isNull()) {
                for(Json::Value::const_iterator iter = mat4s.begin(); iter != mat4s.end(); ++ iter) {
                    const Json::Value& key = iter.key();
                    const Json::Value& value = *iter;

                    Control control;
                    control.name = key.asString();
                    control.handle = glGetUniformLocation(mShaderProg, value.asString().c_str());

                    mUniformMat4s.push_back(control);
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

void ShaderProgramResource::bindModelViewProjMatrices(const glm::mat4& mMat, const glm::mat4& vMat, const glm::mat4& pMat) const {
    if(mUseMMat) {
        glUniformMatrix4fv(mMMatUnif, 1, GL_FALSE, glm::value_ptr(mMat));
    }
    if(mUseVMat) {
        glUniformMatrix4fv(mVMatUnif, 1, GL_FALSE, glm::value_ptr(vMat));
    }
    if(mUsePMat) {
        glUniformMatrix4fv(mPMatUnif, 1, GL_FALSE, glm::value_ptr(pMat));
    }
    if(mUseMVMat) {
        glUniformMatrix4fv(mMVMatUnif, 1, GL_FALSE, glm::value_ptr(vMat * mMat));
    }
    if(mUseVPMat) {
        glUniformMatrix4fv(mVPMatUnif, 1, GL_FALSE, glm::value_ptr(pMat * vMat));
    }
    if(mUseMVPMat) {
        glUniformMatrix4fv(mMVPMatUnif, 1, GL_FALSE, glm::value_ptr(pMat * vMat * mMat));
    }
    if(mUseIMMat) {
        glUniformMatrix4fv(mIMMatUnif, 1, GL_FALSE, glm::value_ptr(glm::inverse(mMat)));
    }
    if(mUseIVMat) {
        glUniformMatrix4fv(mIVMatUnif, 1, GL_FALSE, glm::value_ptr(glm::inverse(vMat)));
    }
    if(mUseIPMat) {
        glUniformMatrix4fv(mIPMatUnif, 1, GL_FALSE, glm::value_ptr(glm::inverse(pMat)));
    }
    if(mUseIMVMat) {
        glUniformMatrix4fv(mIMVMatUnif, 1, GL_FALSE, glm::value_ptr(glm::inverse(vMat * mMat)));
    }
    if(mUseIVPMat) {
        glUniformMatrix4fv(mIVPMatUnif, 1, GL_FALSE, glm::value_ptr(glm::inverse(pMat * vMat)));
    }
    if(mUseIMVPMat) {
        glUniformMatrix4fv(mIMVPMatUnif, 1, GL_FALSE, glm::value_ptr(glm::inverse(pMat * vMat * mMat)));
    }
}
void ShaderProgramResource::bindRenderPass(Renderable::Pass rpc, const glm::mat4& modelMat) const {
    bindModelViewProjMatrices(modelMat, rpc.viewMat, rpc.projMat);
    
    if(mUseScreenSize) {
        glUniform2fv(mScreenSizeUnif, 1, glm::value_ptr(rpc.screenSize));
    }
    if(mUseIScreenSize) {
        glUniform2fv(mIScreenSizeUnif, 1, glm::value_ptr(rpc.invScreenSize));
    }
    if(mUseCameraLoc) {
        glUniform3fv(mCameraLocUnif, 1, glm::value_ptr(rpc.camPos));
    }
    if(mUseCameraDir) {
        glUniform3fv(mCameraDirUnif, 1, glm::value_ptr(rpc.camDir));
    }
}

GLuint ShaderProgramResource::getHandle() const { return mShaderProg; }

bool ShaderProgramResource::needsModelMatrix() const { return mUseMMat; }
bool ShaderProgramResource::needsViewMatrix() const { return mUseVMat; }
bool ShaderProgramResource::needsProjMatrix() const { return mUsePMat; }
bool ShaderProgramResource::needsModelViewMatrix() const { return mUseMVMat; }
bool ShaderProgramResource::needsViewProjMatrix() const { return mUseVPMat; }
bool ShaderProgramResource::needsModelViewProjMatrix() const { return mUseMVPMat; }
bool ShaderProgramResource::needsInvModelMatrix() const { return mUseIMMat; }
bool ShaderProgramResource::needsInvViewMatrix() const { return mUseIVMat; }
bool ShaderProgramResource::needsInvProjMatrix() const { return mUseIPMat; }
bool ShaderProgramResource::needsInvModelViewMatrix() const { return mUseIMVMat; }
bool ShaderProgramResource::needsInvViewProjMatrix() const { return mUseIVPMat; }
bool ShaderProgramResource::needsInvModelViewProjMatrix() const { return mUseIMVPMat; }
bool ShaderProgramResource::needsSunViewProjMatrix() const { return mUseSunViewProjMatrix; }
bool ShaderProgramResource::needsScreenSize() const { return mUseScreenSize; }
bool ShaderProgramResource::needsInvScreenSize() const { return mUseIScreenSize; }
bool ShaderProgramResource::needsCameraLocation() const { return mUseCameraLoc; }
bool ShaderProgramResource::needsCameraDirection() const { return mUseCameraDir; }

GLuint ShaderProgramResource::getModelMatrixUnif() const { return mMMatUnif; }
GLuint ShaderProgramResource::getViewMatrixUnif() const { return mVMatUnif; }
GLuint ShaderProgramResource::getProjMatrixUnif() const { return mPMatUnif; }
GLuint ShaderProgramResource::getModelViewMatrixUnif() const { return mMVMatUnif; }
GLuint ShaderProgramResource::getViewProjMatrixUnif() const { return mVPMatUnif; }
GLuint ShaderProgramResource::getModelViewProjMatrixUnif() const { return mMVPMatUnif; }
GLuint ShaderProgramResource::getInvModelMatrixUnif() const { return mIMMatUnif; }
GLuint ShaderProgramResource::getInvViewMatrixUnif() const { return mIVMatUnif; }
GLuint ShaderProgramResource::getInvProjMatrixUnif() const { return mIPMatUnif; }
GLuint ShaderProgramResource::getInvModelViewMatrixUnif() const { return mIMVMatUnif; }
GLuint ShaderProgramResource::getInvViewProjMatrixUnif() const { return mIVPMatUnif; }
GLuint ShaderProgramResource::getInvModelViewProjMatrixUnif() const { return mIMVPMatUnif; }
GLuint ShaderProgramResource::getSunViewProjMatrixUnif() const { return mSunViewProjMatrixUnif; }
GLuint ShaderProgramResource::getScreenSizeUnif() const { return mScreenSizeUnif; }
GLuint ShaderProgramResource::getInvScreenSizeUnif() const { return mIScreenSizeUnif; }
GLuint ShaderProgramResource::getCameraLocationUnif() const { return mCameraLocUnif; }
GLuint ShaderProgramResource::getCameraDirectionUnif() const { return mCameraDirUnif; }

bool ShaderProgramResource::needsPosAttrib() const { return mUsePosAttrib; }
bool ShaderProgramResource::needsColorAttrib() const { return mUseColorAttrib; }
bool ShaderProgramResource::needsUVAttrib() const { return mUseUVAttrib; }
bool ShaderProgramResource::needsNormalAttrib() const { return mUseNormalAttrib; }
bool ShaderProgramResource::needsTangentAttrib() const { return mUseTangentAttrib; }
bool ShaderProgramResource::needsBitangentAttrib() const { return mUseBitangentAttrib; }

GLuint ShaderProgramResource::getPosAttrib() const { return mPosAttrib; }
GLuint ShaderProgramResource::getColorAttrib() const { return mColorAttrib; }
GLuint ShaderProgramResource::getUVAttrib() const { return mUVAttrib; }
GLuint ShaderProgramResource::getNormalAttrib() const { return mNormalAttrib; }
GLuint ShaderProgramResource::getTangentAttrib() const { return mTangentAttrib; }
GLuint ShaderProgramResource::getBitangentAttrib() const { return mBitangentAttrib; }

const std::vector<ShaderProgramResource::Control>& ShaderProgramResource::getUniformSampler2Ds() const { return mUniformSampler2Ds; }
const std::vector<ShaderProgramResource::Control>& ShaderProgramResource::getUniformFloats() const { return mUniformFloats; }
const std::vector<ShaderProgramResource::Control>& ShaderProgramResource::getUniformInts() const { return mUniformInts; }
const std::vector<ShaderProgramResource::Control>& ShaderProgramResource::getUniformUints() const { return mUniformUints; }
const std::vector<ShaderProgramResource::Control>& ShaderProgramResource::getUniformVec2s() const { return mUniformVec2s; }
const std::vector<ShaderProgramResource::Control>& ShaderProgramResource::getUniformVec3s() const { return mUniformVec3s; }
const std::vector<ShaderProgramResource::Control>& ShaderProgramResource::getUniformVec4s() const { return mUniformVec4s; }
const std::vector<ShaderProgramResource::Control>& ShaderProgramResource::getUniformMat4s() const { return mUniformMat4s; }
const std::vector<ShaderProgramResource::Control>& ShaderProgramResource::getInstancedSampler2Ds() const { return mInstancedSampler2Ds; }
const std::vector<ShaderProgramResource::Control>& ShaderProgramResource::getInstancedFloats() const { return mInstancedFloats; }
const std::vector<ShaderProgramResource::Control>& ShaderProgramResource::getInstancedInts() const { return mInstancedInts; }
const std::vector<ShaderProgramResource::Control>& ShaderProgramResource::getInstancedUints() const { return mInstancedUints; }
const std::vector<ShaderProgramResource::Control>& ShaderProgramResource::getInstancedVec2s() const { return mInstancedVec2s; }
const std::vector<ShaderProgramResource::Control>& ShaderProgramResource::getInstancedVec3s() const { return mInstancedVec3s; }
const std::vector<ShaderProgramResource::Control>& ShaderProgramResource::getInstancedVec4s() const { return mInstancedVec4s; }
const std::vector<ShaderProgramResource::Control>& ShaderProgramResource::getInstancedMat4s() const { return mInstancedMat4s; }

}
