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

#ifndef PGG_CAMERA_HPP
#define PGG_CAMERA_HPP

#include <GraphicsApiLibrary.hpp>

namespace pgg {
class Camera {
public:
    Camera(float fov, float aspect, float nearDepth, float farDepth);
    Camera();
    virtual ~Camera();
    
private:
    float mFOV;
    float mAspect;
    float mNearDepth;
    float mFarDepth;
    
    glm::mat4 mViewMatr;
    glm::mat4 mProjMatr;
    
public:
    void setProjMatrix(glm::mat4 projMatrix);
    void setProjMatrix(float fov, float aspect, float nearDepth, float farDepth);
    void setViewMatrix(glm::mat4 viewMatrix);
    void setViewMatrix(glm::vec3 location, glm::vec3 lookAt, glm::vec3 up);
    
    glm::mat4 getProjMatrix() const;
    glm::mat4 getViewMatrix() const;
    
    float getFOV() const;
    float getFarDepth() const;
    float getNearDepth() const;
    
    glm::vec3 calcLocation() const;
    glm::vec3 calcDirection() const;
};
}

#endif // PGG_CAMERA_HPP
