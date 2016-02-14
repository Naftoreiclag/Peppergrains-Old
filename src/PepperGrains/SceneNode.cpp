#include "SceneNode.hpp"

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
        mWorldTransform = mLocalTransform * mParent->calcWorldTransform();
    } else {
        mWorldTransform = mLocalTransform;
    }

    mWorldTransformDirty = false;

    return mWorldTransform;
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

void SceneNode::grabModel(ModelResource* modelRes) {
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
    calcWorldTransform();

    if(mModelRes) {
        mModelRes->render(viewMat, projMat, mWorldTransform);
    }

    for(std::vector<SceneNode*>::iterator iter = mChildren.begin(); iter != mChildren.end(); ++ iter) {
        SceneNode* child = *iter;
        child->render(viewMat, projMat);
    }
}

void SceneNode::render(const glm::mat4& viewMat, const glm::mat4& projMat, const glm::mat4& modelMat) {
    if(mModelRes) {
        mModelRes->render(viewMat, projMat, modelMat);
    }
}

}
