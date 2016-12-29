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

#include "Camera.hpp"

namespace pgg {

Camera::Camera(float fov, float aspect, float nearDepth, float farDepth) {
    setProjMatrix(fov, aspect, nearDepth, farDepth);
}
Camera::Camera() { }
Camera::~Camera() { }

void Camera::setProjMatrix(glm::mat4 projMatrix) {
    // TODO: retrieve fov, aspect, etc from the provided matrix? How would this work with oblique matrices?
    mProjMatr = projMatrix;
}
void Camera::setProjMatrix(float fov, float aspect, float nearDepth, float farDepth) {
    mFOV = fov;
    mAspect = aspect;
    mNearDepth = nearDepth;
    mFarDepth = farDepth;
    mProjMatr = glm::perspective(mFOV, aspect, mNearDepth, mFarDepth);
}
void Camera::setViewMatrix(glm::mat4 viewMatrix) {
    mViewMatr = viewMatrix;
    // glm::mat4 invCamViewMatr = glm::inverse(viewMatrix);
    // mLocation = glm::vec3(invCamViewMatr * glm::vec4(0.f, 0.f, 0.f, 1.f));
    // mDirection = glm::vec3(invCamViewMatr * glm::vec4(0.f, 0.f, -1.f, 0.f));
}
void Camera::setViewMatrix(glm::vec3 location, glm::vec3 lookAt, glm::vec3 up) {
    mViewMatr = glm::lookAt(location, lookAt - location, up);
}

glm::mat4 Camera::getProjMatrix() const {
    return mProjMatr;
}
glm::mat4 Camera::getViewMatrix() const {
    return mViewMatr;
}
float Camera::getFOV() const {
    return mFOV;
}
float Camera::getFarDepth() const {
    return mFarDepth;
}
float Camera::getNearDepth() const {
    return mNearDepth;
}

glm::vec3 Camera::calcLocation() const {
    // TODO: use transpose instead? assuming the view matrix is orthogonal
    return glm::vec3(glm::inverse(mViewMatr) * glm::vec4(0.f, 0.f, 0.f, 1.f));
}
glm::vec3 Camera::calcDirection() const {
    return glm::vec3(glm::inverse(mViewMatr) * glm::vec4(0.f, 0.f, -1.f, 0.f));
}

}

