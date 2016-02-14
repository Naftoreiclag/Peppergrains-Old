#ifndef SCENENODE_HPP
#define SCENENODE_HPP

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "ModelResource.hpp"

namespace pgg {

class SceneNode {
public:
    SceneNode();
    ~SceneNode();
private:
    glm::vec3 mLocalScale;
    glm::quat mLocalOrientation;
    glm::vec3 mLocalTranslation;

    glm::mat4 mLocalTransform;
    bool mLocalTransformDirty;
    glm::mat4 mWorldTransform;
    bool mWorldTransformDirty;

    ModelResource* mModelRes;

    SceneNode* mParent;
    std::vector<SceneNode*> mChildren;
public:
    const glm::vec3& getLocalScale() const;
    const glm::quat& getLocalOrientation() const;
    const glm::vec3& getLocalTranslation() const;

    const glm::mat4& calcLocalTransform();
    const glm::mat4& calcWorldTransform();

    SceneNode* getParent() const;
    const std::vector<SceneNode*>& getChildren() const;

    void setLocalScale(const glm::vec3& scale);
    void setLocalOrientation(const glm::quat& orientation);
    void setLocalTranslation(const glm::vec3& translation);

    void scale(const glm::vec3& scale);
    void rotate(const glm::quat& rotation);
    void rotate(const glm::vec3& axis, const float& radians);
    void move(const glm::vec3& translation);

    void markWorldTransformDirty();
    void markBothTransformsDirty();

    void grabModel(ModelResource* modelRes);
    void dropModel();
    void render(const glm::mat4& viewMat, const glm::mat4& projMat);
    void render(const glm::mat4& viewMat, const glm::mat4& projMat, const glm::mat4& prgerhtjy);
};

}

#endif // SCENENODE_HPP
