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

#ifndef PGG_MODELINSTANCE_HPP
#define PGG_MODELINSTANCE_HPP

#include <stdint.h>

#include <GraphicsApiStuff.hpp>

#include "Model.hpp"
#include "Spharm.hpp"

// Perhaps all models should be instances, and instead there should just be geometryinstance
// The only problem with this is managing opengl vertex array objects

namespace pgg {
class ModelInstance {
private:
    Model* mModel;
    
    struct Pose {
        Pose(glm::mat4 localTransform, glm::mat4 transform);
        
        glm::mat4 mLocalTransform;
        glm::mat4 mTransform;
        bool mDirty; // if true, then recaculate mTransform as parent->mTransform x this->mLocalTransform
        // Root bones should never be marked as dirty
    };
    std::vector<Pose> mBonePose;
    
public:
    ModelInstance(Model* model);
    ModelInstance();
    
    // Rule of three
    ModelInstance(const ModelInstance& other);
    ModelInstance& operator=(const ModelInstance& other);
    ~ModelInstance();
    
    // Settable render data
    glm::mat4 mModelMatr;
    std::vector<Spharm> mLightprobeData;
    // mFlexWeights;
    
    Model* getModel() const;
    
    void setBonePose(uint8_t id, glm::mat4 transform);
    glm::mat4 getBonePose(uint8_t id);
    void uploadPose();
    
};
}

#endif // PGG_MODELINSTANCE_HPP
