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

#include "Resource.hpp"
#include "ReferenceCounted.hpp"
#include "Geometry.hpp"
#include "Renderable.hpp"
#include "ShaderProgramResource.hpp"

namespace pgg {

// Virtual inheritance to avoid diamond conflict with MaterialResource
class Material : virtual public ReferenceCounted {
public:
    Material();
    virtual ~Material();
    
    static Material* getFallback();
    
    virtual void enableVertexAttributesFor(Geometry* geometry) const = 0;
    virtual bool isVisible(Renderable::Pass rpc) const = 0;
    virtual void useProgram(Renderable::Pass rpc, const glm::mat4& mMat) const = 0;
    virtual const ShaderProgramResource* getShaderProg() const = 0;
};

}

#endif // PGG_MATERIAL_HPP
