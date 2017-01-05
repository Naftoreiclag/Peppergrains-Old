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

#ifndef PGG_MATERIAL_HPP
#define PGG_MATERIAL_HPP

#include "ReferenceCounted.hpp"
#include "Texture.hpp"
#include "Vec3.hpp"

namespace pgg {

/* Represents a list of textures, constant values, and other shader hints to be used in conjunction with a particular
 * Geometry when rendering a Model.
 * 
 * Unlike most resource-y objects, Materials are concrete and can be instantiated using new. This means that custom
 * materials can be created at runtime and modified dynamically. MaterialResource is still immutable, however.
 */
    
// Virtual inheritance to avoid diamond conflict with MaterialResource
class Material : virtual public ReferenceCounted {
public:
    class Input {
    public:
        enum Type {
            TEXTURE,
            VEC3,
            
            EMPTY
        };
    private:
        Type mType;
        union Value {
            Value();
            ~Value();
            
            Texture* mTexture;
            Vec3 mVec3;
        };
        Value mValue;
    public:
        Input(Texture* texture);
        Input(Vec3 vec3);
        Input();
        
        // Rule of three
        Input(const Input& other);
        Input& operator=(const Input& other);
        ~Input();
        
        Type getType() const;
        bool isSpecified() const;
        void clear();
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
    class Technique {
    public:
        enum Type {
            NONE,
            HIGH_LEVEL_VALUES,
            GLSL_SHADER,
        };
        
        Type mType = Type::NONE;
        
        Input mDiffuse;
        Input mSpecular;
        Input mNormals;
        
        void clear();
    };
private:
    Technique mTechnique;
    
public:
    Material();
    Material(Technique technique);
    virtual ~Material();
    
    static Material* getFallback();
    
    virtual void load();
    virtual void unload();
    
    virtual const Technique& getTechnique() const;
};

}

#endif // PGG_MATERIAL_HPP
