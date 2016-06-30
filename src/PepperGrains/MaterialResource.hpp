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

#ifndef PGG_MATERIALRESOURCE_HPP
#define PGG_MATERIALRESOURCE_HPP


#include "json/json.h"
#include "OpenGLStuff.hpp"

#include "Resource.hpp" // Base class: Resource
#include "ShaderProgramResource.hpp"
#include "TextureResource.hpp"

#include "GeometryResource.hpp"

namespace pgg {

class MaterialResource : public Resource {
public:
    struct MaterialInput {
        enum Type {
            NOTHING,
            TEXTURE
        };
        
        Type type;
        
        MaterialInput(const Json::Value& input);
        MaterialInput(TextureResource* textureRes);
        MaterialInput();
        ~MaterialInput();
        
        TextureResource* textureValue;
        
        bool isNothing() const;
    };
    
    /* Different techniques allow for automatic fallback behavior.
     * If for some reason a glsl-shader technique cannot be run,
     * such as in the case of an outdated opengl driver or missing
     * features, the next technique is used as a backup.
     * 
     * As such, only one technique is loaded at a time, and this technique
     * is determined upon loading.
     * 
     * High-level-values is guaranteed to work at least partially, and so
     * should always be the last technique in the list.
     */
    struct Technique {
        enum Type {
            NONE,
            HIGH_LEVEL_VALUES,
            GLSL_SHADER,
        };
        
        Technique();
        ~Technique();
        
        Type type;
        
        MaterialInput* diffuse;
        MaterialInput* specular;
        MaterialInput* normals;
        MaterialInput* ssipgMap;
        MaterialInput* ssipgFlow;
        
        ShaderProgramResource* shaderProg;
    };
    
private:
    Technique mTechnique;

    bool mLoaded;
    
    void loadError();
    bool mIsErrorResource;
    
    void grabNeededHLVShader();

public:
    MaterialResource();
    virtual ~MaterialResource();

public:
    void load();
    void unload();
    
    void enableVertexAttributesFor(GeometryResource* geometry) const;
    void use(const glm::mat4& mMat, const glm::mat4& vMat, const glm::mat4& pMat) const;
    
    Technique::Type getTechniqueType() const;

    const ShaderProgramResource* getShaderProg() const;
};

}

#endif // MATERIALRESOURCE_HPP
