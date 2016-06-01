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
    this->detachAllChildren();
    this->dropModel();
}

void SceneNode::load() {
}

void SceneNode::unload() {
    delete this;
}

const glm::vec3& SceneNode::getLocalScale() const { return mLocalScale; }
const glm::quat& SceneNode::getLocalOrientation() const { return mLocalOrientation; }
const glm::vec3& SceneNode::getLocalTranslation() const { return mLocalTranslation; }
SceneNode* SceneNode::getParent() const { return mParent; }
const std::vector<SceneNode*>& SceneNode::getChildren() const { return mChildren; }

SceneNode* SceneNode::addChild(SceneNode* child) {
    if(child->mParent == this) {
        return child;
    }

    // If the child had a previous parent, then remove child from parent's child list
    if(child->mParent) {
        child->mParent->mChildren.erase(std::remove(mChildren.begin(), mChildren.end(), child), mChildren.end());
        // No need to change child's reference count, as ownership is being transfered
        // Also an increment may not be possible after a decrement since the reference count could drop to zero
    }
    // Child did not previously have a parent, therefore increment reference count
    else {
        child->grab();
    }

    mChildren.push_back(child);
    child->mParent = this;
    child->markWorldTransformDirty();
    
    return child;
}
SceneNode* SceneNode::newChild() {
    return addChild(new SceneNode());
}
void SceneNode::detachChild(SceneNode* child) {
    mChildren.erase(std::remove(mChildren.begin(), mChildren.end(), child), mChildren.end());
    child->mParent = nullptr;
    child->markWorldTransformDirty();
    child->drop();
}
void SceneNode::detachAllChildren() {
    for(std::vector<SceneNode*>::iterator iter = mChildren.begin(); iter != mChildren.end(); ++ iter) {
        SceneNode* child = *iter;
        child->mParent = nullptr;
        child->markWorldTransformDirty();
        child->drop();
    }
    mChildren.clear();
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

SceneNode* SceneNode::calcWorldScale(glm::vec3& scale) {
    if(mParent) {
        const glm::mat4& parentTransform = mParent->calcWorldTransform();
        scale = glm::vec3(parentTransform * glm::vec4(mLocalScale, 0.f));
    }
    else {
        scale = mLocalScale;
    }
    return this;
}
SceneNode* SceneNode::calcWorldOrientation(glm::quat& orientation) {
    if(mParent) {
        // Not completely sure if this calculation is correct
        orientation = glm::quat_cast(this->calcWorldTransform());
    }
    else {
        orientation = mLocalOrientation;
    }
    return this;
}
SceneNode* SceneNode::calcWorldTranslation(glm::vec3& translation) {
    if(mParent) {
        const glm::mat4& parentTransform = mParent->calcWorldTransform();
        translation = glm::vec3(parentTransform * glm::vec4(mLocalTranslation, 1.f));
    }
    else {
        translation = mLocalTranslation;
    }
    return this;
}

SceneNode* SceneNode::setLocalScale(const glm::vec3& scale) {
    mLocalScale = scale;
    this->markBothTransformsDirty();
    return this;
}
SceneNode* SceneNode::setLocalOrientation(const glm::quat& orientation) {
    mLocalOrientation = orientation;
    this->markBothTransformsDirty();
    return this;
}
SceneNode* SceneNode::setLocalTranslation(const glm::vec3& translation) {
    mLocalTranslation = translation;
    this->markBothTransformsDirty();
    return this;
}
SceneNode* SceneNode::scale(const glm::vec3& scale) {
    mLocalScale *= scale;
    this->markBothTransformsDirty();
    return this;
}
SceneNode* SceneNode::scale(const float& scale) {
    mLocalScale *= scale;
    this->markBothTransformsDirty();
    return this;
}
SceneNode* SceneNode::rotate(const glm::quat& rotation) {
    mLocalOrientation = rotation * mLocalOrientation;
    this->markBothTransformsDirty();
    return this;
}
SceneNode* SceneNode::rotate(const glm::vec3& axis, const float& radians) {
    mLocalOrientation = glm::rotate(mLocalOrientation, radians, axis);
    this->markBothTransformsDirty();
    return this;
}
SceneNode* SceneNode::rotatePitch(const float& radians) {
    this->rotate(glm::vec3(1.f, 0.f, 0.f), radians);
    this->markBothTransformsDirty();
    return this;
}
SceneNode* SceneNode::rotateYaw(const float& radians) {
    this->rotate(glm::vec3(0.f, 1.f, 0.f), radians);
    this->markBothTransformsDirty();
    return this;
}
SceneNode* SceneNode::rotateRoll(const float& radians) {
    this->rotate(glm::vec3(0.f, 0.f, 1.f), radians);
    this->markBothTransformsDirty();
    return this;
}
SceneNode* SceneNode::move(const glm::vec3& translation) {
    mLocalTranslation += translation;
    this->markBothTransformsDirty();
    return this;
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

SceneNode* SceneNode::grabModel(Model* modelRes) {
    this->dropModel();

    mModelRes = modelRes;
    mModelRes->grab();
    return this;
}
SceneNode* SceneNode::dropModel() {
    if(mModelRes) {
        mModelRes->drop();
        mModelRes = nullptr;
    }
    return this;
}

void SceneNode::render(const Model::RenderPassConfiguration& rendPass) {
    this->calcWorldTransform();

    if(mModelRes) {
        mModelRes->render(rendPass, mWorldTransform);
    }

    // Render all children
    for(std::vector<SceneNode*>::iterator iter = mChildren.begin(); iter != mChildren.end(); ++ iter) {
        SceneNode* child = *iter;
        child->render(rendPass);
    }
}

}
