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

#include "ModelInstance.hpp"

namespace pgg {


ModelInstance::Pose::Pose(glm::mat4 localTransform, glm::mat4 transform)
: mLocalTransform(localTransform)
, mTransform(transform)
, mDirty(false) { }

ModelInstance::ModelInstance(Model* model)
: mModel(model) {
    mModel->grab();
    const Geometry* geom = mModel->getGeometry();
    mLightprobeData.resize(geom->getLightprobes().size());
    const std::vector<Geometry::Armature::Bone>& boneList = geom->getArmature().mBones;
    mBonePose.reserve(boneList.size());
    for(const Geometry::Armature::Bone& bone : boneList) {
        mBonePose.push_back(Pose(bone.mLocalTransform, bone.mTransform));
    }
}

ModelInstance::ModelInstance()
: mModel(Model::getFallback()) {
    mModel->grab();
}

// Copy constructor
ModelInstance::ModelInstance(const ModelInstance& other)
: mModel(other.mModel)
, mModelMatr(other.mModelMatr)
, mLightprobeData(other.mLightprobeData)
, mBonePose(other.mBonePose) {
    mModel->grab();
}

// Copy assignment
ModelInstance& ModelInstance::operator=(const ModelInstance& other) {
    if(other.mModel != mModel) {
        mModel->drop();
        mModel = other.mModel;
        mModel->grab();
    }
    mModelMatr = other.mModelMatr;
    mLightprobeData = other.mLightprobeData;
    
    return *this;
}

// Deconstructor
ModelInstance::~ModelInstance() {
    mModel->drop();
}

Model* ModelInstance::getModel() const {
    return mModel;
}

void ModelInstance::setBonePose(uint8_t thisId, glm::mat4 transform) {
    // TODO: range check
    const std::vector<Geometry::Armature::Bone>& bones = mModel->getGeometry()->getArmature().mBones;
    
    Pose& thisPose = mBonePose.at(thisId);
    const Geometry::Armature::Bone& thisBone = bones.at(thisId);
    
    // Set this local transform
    thisPose.mLocalTransform = transform;
    
    // Mark all children as dirty ----
    // If a bone is dirty, then all of its descendants are also dirty, so no need to iterate
    // (Likewise, if a node is clean, then all of its ancestors are also clean)
    // Therefore only if a bone is clean (!mDirty) should there be iteration over its children
    if(!thisPose.mDirty) {
        thisPose.mDirty = true;
        std::vector<uint8_t> markList = thisBone.mChildren;
        while(markList.size() > 0) {
            std::vector<uint8_t> nextList;
            for(uint8_t childId : markList) {
                Pose& childPose = mBonePose.at(childId);
                
                if(!childPose.mDirty) {
                    childPose.mDirty = true;
                    const Geometry::Armature::Bone& childBone = bones.at(childId);
                    nextList.insert(nextList.end(), childBone.mChildren.begin(), childBone.mChildren.end());
                }
            }
            markList = nextList;
        }
    }
}

glm::mat4 ModelInstance::getBonePose(uint8_t id) {
    Pose& pose = mBonePose.at(id);
    if(pose.mDirty) {
        const Geometry::Armature::Bone& bone = mModel->getGeometry()->getArmature().mBones.at(id);
        if(bone.mHasParent) {
            pose.mTransform = getBonePose(bone.mParent) * pose.mLocalTransform;
        } else {
            pose.mTransform = pose.mLocalTransform;
        }
    }
    
    return pose.mTransform;
}

void ModelInstance::uploadPose() {
    if(mModel->getGeometry()->getArmature().mBones.size() == 0) {
        return;
    }
    
    // ...
}

}

