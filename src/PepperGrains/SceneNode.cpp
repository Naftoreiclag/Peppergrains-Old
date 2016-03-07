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

#include "SceneNode.hpp"

#include <algorithm>

#include <OpenGLStuff.hpp>

namespace pgg {

SceneNode::SceneNode()
: mLocalTransformDirty(false)
, mWorldTransformDirty(true)
, mParent(nullptr)
, mModelRes(nullptr)
, mLocalScale(glm::vec3(1.f))
, mLocalTranslation(glm::vec3(0.f)) {
}

SceneNode::~SceneNode() {
}

const glm::vec3& SceneNode::getLocalScale() const { return mLocalScale; }
const glm::quat& SceneNode::getLocalOrientation() const { return mLocalOrientation; }
const glm::vec3& SceneNode::getLocalTranslation() const { return mLocalTranslation; }
SceneNode* SceneNode::getParent() const { return mParent; }
const std::vector<SceneNode*>& SceneNode::getChildren() const { return mChildren; }

void SceneNode::addChild(SceneNode* child) {
    if(child->mParent == this) {
        return;
    }

    if(child->mParent) {
        child->mParent->mChildren.erase(std::remove(mChildren.begin(), mChildren.end(), child), mChildren.end());
    }

    mChildren.push_back(child);
    child->mParent = this;
    child->markWorldTransformDirty();
}
void SceneNode::detachChild(SceneNode* child) {
    mChildren.erase(std::remove(mChildren.begin(), mChildren.end(), child), mChildren.end());
    child->mParent = nullptr;
    child->markWorldTransformDirty();
}

const glm::mat4& SceneNode::calcLocalTransform() {
    if(!mLocalTransformDirty) {
        return mLocalTransform;
    }

    mLocalTransform =
        glm::translate(glm::mat4(1.f), mLocalTranslation) *
        glm::mat4_cast(mLocalOrientation) *
        glm::scale(glm::mat4(1.f), mLocalScale);

    mLocalTransformDirty = false;
    mWorldTransformDirty = true;

    return mLocalTransform;
}
const glm::mat4& SceneNode::calcWorldTransform() {
    if(!mWorldTransformDirty) {
        return mWorldTransform;
    }

    this->calcLocalTransform();

    if(mParent) {
        mWorldTransform = mParent->calcWorldTransform() * mLocalTransform;
    } else {
        mWorldTransform = mLocalTransform;
    }

    mWorldTransformDirty = false;

    return mWorldTransform;
}

void SceneNode::calcWorldScale(glm::vec3& scale) {
    if(mParent) {
        const glm::mat4& parentTransform = mParent->calcWorldTransform();
        // Not completely sure if this calculation is correct
        scale = glm::vec3(parentTransform * glm::vec4(mLocalScale, 0.f));
    }
    else {
        scale = mLocalScale;
    }
}
void SceneNode::calcWorldOrientation(glm::quat& orientation) {
    if(mParent) {
        // Not completely sure if this calculation is correct
        orientation = glm::quat_cast(this->calcWorldTransform());
    }
    else {
        orientation = mLocalOrientation;
    }
}
void SceneNode::calcWorldTranslation(glm::vec3& translation) {
    if(mParent) {
        const glm::mat4& parentTransform = mParent->calcWorldTransform();
        translation = glm::vec3(parentTransform * glm::vec4(mLocalTranslation, 1.f));
    }
    else {
        translation = mLocalTranslation;
    }
}

void SceneNode::setLocalScale(const glm::vec3& scale) {
    mLocalScale = scale;
    this->markBothTransformsDirty();
}
void SceneNode::setLocalOrientation(const glm::quat& orientation) {
    mLocalOrientation = orientation;
    this->markBothTransformsDirty();
}
void SceneNode::setLocalTranslation(const glm::vec3& translation) {
    mLocalTranslation = translation;
    this->markBothTransformsDirty();
}
void SceneNode::scale(const glm::vec3& scale) {
    mLocalScale *= scale;
    this->markBothTransformsDirty();
}
void SceneNode::rotate(const glm::quat& rotation) {
    mLocalOrientation = rotation * mLocalOrientation;
    this->markBothTransformsDirty();
}
void SceneNode::rotate(const glm::vec3& axis, const float& radians) {
    mLocalOrientation = glm::rotate(mLocalOrientation, radians, axis);
    this->markBothTransformsDirty();
}
void SceneNode::move(const glm::vec3& translation) {
    mLocalTranslation += translation;
    this->markBothTransformsDirty();
}
void SceneNode::markWorldTransformDirty() {
    // Avoid unnecessary tree iteration
    if(mWorldTransformDirty) {
        return;
    }

    mWorldTransformDirty = true;
    for(std::vector<SceneNode*>::iterator iter = mChildren.begin(); iter != mChildren.end(); ++ iter) {
        SceneNode* child = *iter;
        child->markWorldTransformDirty();
    }
}
void SceneNode::markBothTransformsDirty() {
    mLocalTransformDirty = true;
    this->markWorldTransformDirty();
}

void SceneNode::grabModel(Model* modelRes) {
    this->dropModel();

    mModelRes = modelRes;
    modelRes->grab();
}
void SceneNode::dropModel() {
    if(mModelRes) {
        mModelRes->drop();
        mModelRes = nullptr;
    }
}

void SceneNode::render(const glm::mat4& viewMat, const glm::mat4& projMat) {
    this->calcWorldTransform();

    if(mModelRes) {
        mModelRes->render(viewMat, projMat, mWorldTransform);
    }

    // Render all children
    for(std::vector<SceneNode*>::iterator iter = mChildren.begin(); iter != mChildren.end(); ++ iter) {
        SceneNode* child = *iter;
        child->render(viewMat, projMat);
    }
}

}
