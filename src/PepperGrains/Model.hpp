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

#ifndef PGG_MODEL_HPP
#define PGG_MODEL_HPP

#include "ReferenceCounted.hpp"
#include "Resource.hpp"
#include "Renderable.hpp"
#include "Material.hpp"

namespace pgg {

// Virtual inheritance to avoid diamond conflict with ModelResource
class Model : virtual public ReferenceCounted {
public:
    static Model* getFallback();

    virtual void render(Renderable::Pass rendPass, const glm::mat4& modelMat) = 0;
    
    virtual Geometry* getGeometry() const = 0;
    virtual Material* getMaterial() const = 0;
    
    virtual void bindVertexArray() = 0;
};

class FallbackModel : public Model {
public:
    void render(Renderable::Pass rendPass, const glm::mat4& modelMat);
    
    void load();
    void unload();
    
    Geometry* getGeometry() const;
    Material* getMaterial() const;
    
    void bindVertexArray();
    
};

}

#endif // PGG_MODEL_HPP
