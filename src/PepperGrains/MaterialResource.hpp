/*
   Copyright 2015-2017 James Fong

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


#include <json/json.h>
#include <GraphicsApiLibrary.hpp>

#include "Resource.hpp" // Base class: Resource
#include "ShaderProgramResource.hpp"
#include "Texture.hpp"
#include "Model.hpp"
#include "Material.hpp"

namespace pgg {

class MaterialResource : public Material, public Resource {
private:
    Material::Technique mTechnique;
    bool mLoaded;
public:
    MaterialResource();
    virtual ~MaterialResource();
    
    static Material::Input jsonToMaterialInput(const Json::Value& val);
    static Material* gallop(Resource* resource);

    void load();
    void unload();
};

}

#endif // MATERIALRESOURCE_HPP
