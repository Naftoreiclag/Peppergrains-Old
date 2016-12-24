/*
   Copyright 2016 James Fong

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

#ifndef PGG_MODELINSTANCE_HPP
#define PGG_MODELINSTANCE_HPP

#include "Model.hpp"
#include "OpenGLStuff.hpp"

namespace pgg {
class ModelInstance {
private:
    Model* mModel;
public:
    ModelInstance(Model* model);
    ModelInstance();
    
    // Rule of three
    ModelInstance(const ModelInstance& other);
    ModelInstance& operator=(const ModelInstance& other);
    ~ModelInstance();
    
    glm::mat4 mModelMatr;
    
    Model* getModel() const;
    
};
}

#endif // PGG_MODELINSTANCE_HPP
