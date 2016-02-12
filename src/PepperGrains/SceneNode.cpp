#include "SceneNode.hpp"

#include <OpenGLStuff.hpp>

SceneNode::SceneNode()
: mLocalTransformDirty(false)
, mWorldTransformDirty(true)
, mParent(nullptr) {
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
        glm::scale(glm::mat4(1.f), mLocalScale) *
        glm::mat4_cast(mLocalOrientation) *
        glm::translate(glm::mat4(1.f), mLocalTranslation);

    mLocalTransformDirty = false;
    mWorldTransformDirty = true;

    return mLocalTransform;
}
const glm::mat4& SceneNode::calcWorldTransform() {
    if(!mWorldTransformDirty) {
        return mWorldTransform;
    }

    this->calcLocalTransform();

    mWorldTransform = mLocalTransform * mParent->calcWorldTransform();

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
void SceneNode::move(const glm::vec3& translation) {
    mLocalTranslation += translation;
    this->markBothTransformsDirty();
}
void SceneNode::markWorldTransformDirty() {
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

void SceneNode::render(const glm::mat4& viewMat, const glm::mat4& projMat) {
    calcWorldTransform();



    for(std::vector<SceneNode*>::iterator iter = mChildren.begin(); iter != mChildren.end(); ++ iter) {
        SceneNode* child = *iter;
        child->render(viewMat, projMat);
    }
}
