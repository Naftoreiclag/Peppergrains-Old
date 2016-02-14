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

#ifndef PGG_SCENENODE_HPP
#define PGG_SCENENODE_HPP

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
