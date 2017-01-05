/*
   Copyright 2016-2017 James Fong

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

#include "Material.hpp"

#include "Logger.hpp"
#include "MaterialResource.hpp"

namespace pgg {

Material::Input::Value::Value()
: mTexture(nullptr){
}
Material::Input::Value::~Value() { }

Material::Input::Input(Texture* texture)
: mType(Type::TEXTURE) {
    if(!texture) {
        mType = Type::EMPTY;
        return;
    }
    mValue.mTexture = texture;
    mValue.mTexture->grab();
}
Material::Input::Input(Vec3 vec3)
: mType(Type::VEC3) {
    mValue.mVec3 = vec3;
}
Material::Input::Input()
: mType(Type::EMPTY) {
}

// Copy constructor
Material::Input::Input(const Material::Input& other)
: mType(other.mType) {
    switch(mType) {
        case Type::TEXTURE: {
            mValue.mTexture = other.mValue.mTexture;
            mValue.mTexture->grab();
            break;
        }
        case Type::VEC3: {
            mValue.mVec3 = other.mValue.mVec3;
            break;
        }
        default: break;
    }
}

// Copy assignment
Material::Input& Material::Input::operator=(const Material::Input& other) {
    // Retain same type, use new value
    if(mType) {
        switch(mType) {
            case Type::TEXTURE: {
                // Only do reference recounting if the new texture is different
                if(mValue.mTexture != other.mValue.mTexture) {
                    mValue.mTexture->drop();
                    mValue.mTexture = other.mValue.mTexture;
                    mValue.mTexture->grab();
                }
                break;
            }
            case Type::VEC3: {
                mValue.mVec3 = other.mValue.mVec3;
                break;
            }
            default: break;
        }
    }
    
    // Changing to new type
    else {
        switch(mType) {
            case Type::TEXTURE: {
                mValue.mTexture->drop();
                break;
            }
            default: break;
        }
        mType = other.mType;
        switch(mType) {
            case Type::TEXTURE: {
                mValue.mTexture = other.mValue.mTexture;
                mValue.mTexture->grab();
                break;
            }
            case Type::VEC3: {
                mValue.mVec3 = other.mValue.mVec3;
                break;
            }
            default: break;
        }
    }
}

Material::Input::Type Material::Input::getType() const { return mType; }
bool Material::Input::isSpecified() const { return mType != Type::EMPTY; }

// Deconstructor
Material::Input::~Input() {
    clear();
}

void Material::Input::clear() {
    switch(mType) {
        case Type::TEXTURE: {
            mValue.mTexture->drop();
            break;
        }
        default: break;
    }
    mType = Type::EMPTY;
}

void Material::Technique::clear() {
    mDiffuse.clear();
    mNormals.clear();
    mSpecular.clear();
    mType = Type::NONE;
}

Material::Material() { }
Material::Material(Material::Technique technique)
: mTechnique(technique) {
}
Material::~Material() { }

Material* Material::getFallback() {
    // Not giving a file name for MaterialResource results in the special error type resource
    static MaterialResource fallbackMaterial;
    return &fallbackMaterial;
}

void Material::load() { }
void Material::unload() { delete this; }

const Material::Technique& Material::getTechnique() const {
    return mTechnique;
}

}

