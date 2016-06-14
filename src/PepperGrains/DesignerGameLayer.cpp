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

#include "DesignerGameLayer.hpp"

#include <limits>
#include <algorithm>
#include <cassert>
#include <cmath>
#include <math.h>
#include <iostream>
#include <sstream>

#include "glm/gtx/string_cast.hpp"
#include "SDL2/SDL.h"

#include "AxesModel.hpp"
#include "InputMoveESignal.hpp"
#include "DebugFPControllerEListe.hpp"
#include "SceneNodeEComp.hpp"
#include "SceneNodeESys.hpp"
#include "SunLightModel.hpp"
#include "DirectionalLightModel.hpp"
#include "PointLightModel.hpp"
#include "RigidBodyEComp.hpp"
#include "GrassModel.hpp"
#include "TessModel.hpp"
#include "Vec3.hpp"

namespace pgg {
    
float nearestRightAngle(const float& radians) {
    if(radians == 0.f) {
        return 0.f;
    }
    
    const float full = 6.2831f;
    
    float wrap = fmod(fmod(radians, full) + full, full);
    
    if(wrap < 0.7854f) {
        return 0.0000f;
    } else if(wrap < 2.3562f) {
        return 1.5708f;
    } else if(wrap < 3.9270f) {
        return 3.1416f;
    } else if(wrap < 5.4978f) {
        return 4.7124f;
    } else {
        return 0.0000f;
    }
}
    
float nearestPointOn3DLine(const Vec3& s1, const Vec3& s2, const Vec3& m1, const Vec3& m2) {
    Vec3 mouse = m2 - m1;
    Vec3 street = s2 - s1;
    float mouseMagSq = mouse.magSq();
    float streetMagSq = street.magSq();
    
    Vec3 originDisp = s1 - m1;
    
    float originDotMouse = originDisp.dot(mouse);
    float originDotStreet = originDisp.dot(street);
    float mouseDotStreet = mouse.dot(street);
    
    float divisor = mouseMagSq * streetMagSq - mouseDotStreet * mouseDotStreet;
    if(divisor == 0.f) {
        return std::numeric_limits<float>::infinity();
    } else {
        return (originDotMouse * mouseDotStreet - originDotStreet * mouseMagSq) / divisor;
    }
}
    
float toNearestMultiple(const float& a, const float& b) {
    return std::floor((a / b) + 0.5) * b;
}

float cotangent(const float& a) {
    return std::cos(a) / std::sin(a);
}

// TODO: this
glm::quat quaternionLookAt(Vec3 targetDirection, Vec3 initialDirection, Vec3 upDirection) {
    
    return glm::quat();
}

DesignerGameLayer::Edge::Edge() {
    
}

DesignerGameLayer::Edge::~Edge() {
    
}

DesignerGameLayer::StraightEdge::StraightEdge(const Vec3& start, const Vec3& end)
: mStartLoc(start)
, mEndLoc(end) {
}

DesignerGameLayer::StraightEdge::~StraightEdge() {
    
}

bool DesignerGameLayer::StraightEdge::canBindTo(Edge* other) const {
    return false;
}

DesignerGameLayer::Manipulator::Manipulator()
: handleDragged(-1)
, handleHovered(-1) {
}
DesignerGameLayer::Manipulator::~Manipulator() {
    
}
    
DesignerGameLayer::Plate::Plate()
: mIntegralX(0)
, mIntegralY(0)
, mIntegralZ(0) { }
DesignerGameLayer::Plate::~Plate() { }

void DesignerGameLayer::Plate::setIntermediatePitch(float radians) {
    intermediateYaw = 0;
    intermediateRoll = 0;
    
    intermediatePitch = radians;
}
void DesignerGameLayer::Plate::setIntermediateYaw(float radians) {
    intermediatePitch = 0;
    intermediateRoll = 0;
    
    intermediateYaw = radians;
}
void DesignerGameLayer::Plate::setIntermediateRoll(float radians) {
    intermediatePitch = 0;
    intermediateYaw = 0;
    
    intermediateRoll = radians;
}
void DesignerGameLayer::Plate::finalizeRotation() {
    intermediatePitch = nearestRightAngle(intermediatePitch);
    intermediateYaw = nearestRightAngle(intermediateYaw);
    intermediateRoll = nearestRightAngle(intermediateRoll);
    
    if(intermediatePitch != 0.f) {
        mTargetOrientation = glm::angleAxis(intermediatePitch, glm::vec3(1.f, 0.f, 0.f)) * mTargetOrientation;
    }
    if(intermediateYaw != 0.f) {
        mTargetOrientation = glm::angleAxis(intermediateYaw, glm::vec3(0.f, 1.f, 0.f)) * mTargetOrientation;
    }
    if(intermediateRoll != 0.f) {
        mTargetOrientation = glm::angleAxis(intermediateRoll, glm::vec3(0.f, 0.f, 1.f)) * mTargetOrientation;
    }
    
    // std::cout << mTargetOrientation.x << "\t" << mTargetOrientation.y << "\t" << mTargetOrientation.z << "\t" << mTargetOrientation.w << std::endl;
    
    intermediatePitch = 0;
    intermediateYaw = 0;
    intermediateRoll = 0;
}

void DesignerGameLayer::selectPlate(Plate* plate) {
    mPlateSelected = plate;
}

void DesignerGameLayer::deselectPlate() {
    mPlateSelected = nullptr;
}

DesignerGameLayer::CollisionWorldPackage::CollisionWorldPackage() {

    mBroadphase = new btDbvtBroadphase();
    mCollisionConfiguration = new btDefaultCollisionConfiguration();
    mDispatcher = new btCollisionDispatcher(mCollisionConfiguration);
    mCollisionWorld = new btCollisionWorld(mDispatcher, mBroadphase, mCollisionConfiguration);
    
}

DesignerGameLayer::CollisionWorldPackage::~CollisionWorldPackage() {
    
    delete mCollisionWorld;
    delete mDispatcher;
    delete mCollisionConfiguration;
    delete mBroadphase;
    
}

Vec3 DesignerGameLayer::Plate::getLocation() const {
    return Vec3(
        ((float) mIntegralX) * (1.f / 12.f),
        ((float) mIntegralY) * (1.f / 12.f),
        ((float) mIntegralZ) * (1.f / 12.f)
    );
}

Vec3 DesignerGameLayer::Plate::getRenderLocation() const {
    return mRenderLocation;
}

void DesignerGameLayer::Plate::setLocation(Vec3 location, float snapSize) {
    mIntegralX = (uint32_t) std::floor(toNearestMultiple(location.x, snapSize) * 12.f + 0.5);
    mIntegralY = (uint32_t) std::floor(toNearestMultiple(location.y, snapSize) * 12.f + 0.5);
    mIntegralZ = (uint32_t) std::floor(toNearestMultiple(location.z, snapSize) * 12.f + 0.5);
}

void DesignerGameLayer::Plate::tick(float tpf) {
    Vec3 target((float) mIntegralX, (float) mIntegralY, (float) mIntegralZ);
    target /= 12.f;
    
    // Exponential decay
    if((target - mRenderLocation).mag() < tpf) {
        mRenderLocation = target;
    }
    else {
        mRenderLocation += (target - mRenderLocation) * tpf * 15.f;
    }
    
    collisionObject->getWorldTransform().setOrigin(mRenderLocation);
    collisionWorld->removeCollisionObject(collisionObject);
    collisionWorld->addCollisionObject(collisionObject);
    
    float tempPitch = nearestRightAngle(intermediatePitch);
    float tempYaw = nearestRightAngle(intermediateYaw);
    float tempRoll = nearestRightAngle(intermediateRoll);
    
    glm::quat finalRenderOrientation = mTargetOrientation;
    
    if(tempPitch != 0.f) {
        finalRenderOrientation = glm::angleAxis(tempPitch, glm::vec3(1.f, 0.f, 0.f)) * finalRenderOrientation;
    }
    if(tempYaw != 0.f) {
        finalRenderOrientation = glm::angleAxis(tempYaw, glm::vec3(0.f, 1.f, 0.f)) * finalRenderOrientation;
    }
    if(tempRoll != 0.f) {
        finalRenderOrientation = glm::angleAxis(tempRoll, glm::vec3(0.f, 0.f, 1.f)) * finalRenderOrientation;
    }
    
    mRenderOrientation = glm::slerp(mRenderOrientation, finalRenderOrientation, tpf * 15.f);
    
    mSceneNode->setLocalTranslation(mRenderLocation);
    mSceneNode->setLocalOrientation(mRenderOrientation);
}

DesignerGameLayer::DesignerGameLayer(uint32_t width, uint32_t height)
: mScreenWidth(width)
, mScreenHeight(height) {
}

DesignerGameLayer::~DesignerGameLayer() {
}

void DesignerGameLayer::newPlate() {
    Plate* plate = new Plate();
    
    ResourceManager* resman = ResourceManager::getSingleton();
    
    plate->mSceneNode = mRootNode->newChild();
    plate->mSceneNode->grab();
    plate->mSceneNode->grabModel(resman->findModel("SquarePlate.model"));
    
    plate->collisionShape = new btBoxShape(Vec3(0.5f, 1.f / 60.f, 0.5f));
    plate->motionState = new btDefaultMotionState();
    plate->collisionObject = new btCollisionObject();
    plate->collisionObject->setCollisionShape(plate->collisionShape);
    plate->collisionObject->setUserPointer(plate);
    plate->collisionObject->getWorldTransform().setOrigin(Vec3(0.f, 0.f, 0.f));
    
    mCollisionWorld->addCollisionObject(plate->collisionObject);
    plate->collisionWorld = mCollisionWorld;
    
    // Set up edges
    {
        plate->mEdges.push_back(new StraightEdge(Vec3(0.5f, 0.f, 0.5f), Vec3(0.5f, 0.f, -0.5f)));
        plate->mEdges.push_back(new StraightEdge(Vec3(0.5f, 0.f, -0.5f), Vec3(-0.5f, 0.f, 0.5f)));
        plate->mEdges.push_back(new StraightEdge(Vec3(-0.5f, 0.f, 0.5f), Vec3(-0.5f, 0.f, -0.5f)));
        plate->mEdges.push_back(new StraightEdge(Vec3(-0.5f, 0.f, -0.5f), Vec3(0.5f, 0.f, 0.5f)));
    }
    
    mPlates.push_back(plate);
}

void DesignerGameLayer::deletePlate(Plate* plate) {
    mPlates.erase(std::remove(mPlates.begin(), mPlates.end(), plate), mPlates.end());
    
    plate->mSceneNode->drop();
    
    delete plate->collisionObject;
    delete plate->motionState;
    delete plate->collisionShape;
    
    delete plate;
}

// Lifecycle
void DesignerGameLayer::onBegin() {
    SDL_SetRelativeMouseMode(SDL_FALSE);
    
    mCollisionWorld = mCollisionPackage[0].mCollisionWorld;
    mManipulatorCollisionWorld = mCollisionPackage[1].mCollisionWorld;
    
    ResourceManager* resman = ResourceManager::getSingleton();
    
    mRootNode = new SceneNode();
    mRootNode->grab();
    
    mRenderer = new DeferredRenderer(mScreenWidth, mScreenHeight);
    mRenderer->grab();
    
    mCameraSpeedMin = 2.0f;
    mCameraSpeedMax = 10.f;
    mCameraAcceleration = 1.0f;
    mCameraSpeed = mCameraSpeedMin;
    
    mCamLocNode = new SceneNode();
    mCamLocNode->move(Vec3(0.f, 5.f, 5.f));
    
    mCamRollNode = new SceneNode();
    mCamPitchNode = new SceneNode();
    mCamYawNode = new SceneNode();
    mCamLocNode->addChild(mCamYawNode);
    mCamYawNode->addChild(mCamPitchNode);
    mCamPitchNode->addChild(mCamRollNode);
    
    mCamPitchNode->rotatePitch(glm::radians(-45.f));
    
    mRenderer->setCameraProjection(glm::radians(50.f), 0.2f, 200.f);
    
    mRenderer->setSunDirection(glm::vec3(-3.f, -5.f, -2.f));
    
    
    //mManipulator = resman->findModel("ManipulatorArrow.model");
    
    mDebugCube = mRootNode->newChild();
    mDebugCube->grabModel(resman->findModel("RoseCube.model"));
    mDebugCube->setLocalScale(Vec3(0.2, 0.2, 0.2));
    mDebugCube->grab();
    mDebugCube->setLocalTranslation(Vec3(999, 999, 999));
    
    loadSlimeShader();
    loadManipulator();
    
    mInfCheck = new InfiniteCheckerboardModel();
    mInfCheck->grab();
    mRootNode->newChild()->grabModel(mInfCheck);

    fps = 0.f;
    fpsWeight = 0.85f;
    
    mDebugWireframe = false;

    oneSecondTimer = 0.f;
    
    mPlateFreeDragged = nullptr;
    mPlateSelected = nullptr;
    
    mGridSize = 6;
    
    cyclicSawtooth = 0.f;
    cyclicSinusodal = 0.f;
    
    newPlate();
    newPlate();
    newPlate();
    newPlate();
    newPlate();
    newPlate();
    newPlate();
    newPlate();
    newPlate();
}

void DesignerGameLayer::updateManipulatorTransform() {
    if(!mPlateSelected || mPlateFreeDragged) {
        return;
    }
    
    glm::mat4 cameraMatrix = mRenderer->getCameraProjectionMatrix() * mRenderer->getCameraViewMatrix();
    
    glm::vec4 asdf = cameraMatrix * glm::vec4(glm::vec3(mPlateSelected->getRenderLocation()), 1.f);
    asdf /= asdf.w;

    float z = asdf.z * 2.f - 1.f;
    const float& near = mRenderer->getCameraNearDepth();
    const float& far = mRenderer->getCameraFarDepth();
    float linDepth = (2.f * near * far) / (far + near - z * (far - near));

    float clipRad = cotangent(mRenderer->getCameraFOV() / 2.f) / linDepth;
    
    mManipulator.scale = 0.5f / clipRad;
    
    if(mManipulator.scale < 0.5f) {
        mManipulator.scale = 0.5f;
    }
    mManipulator.location = mPlateSelected->getRenderLocation();
}

void DesignerGameLayer::updateManipulatorPhysics() {
    updateManipulatorTransform();
    if(!mPlateSelected || mPlateFreeDragged) {
        return;
    }
    
    for(uint8_t i = 0; i < 6; ++ i) {
        mManipulatorCollisionWorld->removeCollisionObject(mManipulator.collisionObjects[i]);
        
        delete mManipulator.collisionShapes[i];
        
        if(i == 0) {
            mManipulator.collisionShapes[i] = new btCylinderShapeX(Vec3(0.625f, 0.05f, 0.05f) * mManipulator.scale);
            mManipulator.collisionObjects[i]->getWorldTransform().setOrigin(Vec3(0.775f, 0.f, 0.f) * mManipulator.scale + mManipulator.location);
        } else if(i == 1) {
            mManipulator.collisionShapes[i] = new btCylinderShape(Vec3(0.05f, 0.625f, 0.05f) * mManipulator.scale);
            mManipulator.collisionObjects[i]->getWorldTransform().setOrigin(Vec3(0.f, 0.775f, 0.f) * mManipulator.scale + mManipulator.location);
        } else if(i == 2) {
            mManipulator.collisionShapes[i] = new btCylinderShapeZ(Vec3(0.05f, 0.05f, 0.625f) * mManipulator.scale);
            mManipulator.collisionObjects[i]->getWorldTransform().setOrigin(Vec3(0.f, 0.f, 0.775f) * mManipulator.scale + mManipulator.location);
        } else if(i == 3) {
            mManipulator.collisionShapes[i] = new btCylinderShapeX(Vec3(0.05f, 0.3f, 0.3f) * mManipulator.scale);
            mManipulator.collisionObjects[i]->getWorldTransform().setOrigin(Vec3(1.65f, 0.f, 0.f) * mManipulator.scale + mManipulator.location);
        } else if(i == 4) {
            mManipulator.collisionShapes[i] = new btCylinderShape(Vec3(0.3f, 0.05f, 0.3f) * mManipulator.scale);
            mManipulator.collisionObjects[i]->getWorldTransform().setOrigin(Vec3(0.f, 1.65f, 0.f) * mManipulator.scale + mManipulator.location);
        } else if(i == 5) {
            mManipulator.collisionShapes[i] = new btCylinderShapeZ(Vec3(0.3f, 0.3f, 0.05f) * mManipulator.scale);
            mManipulator.collisionObjects[i]->getWorldTransform().setOrigin(Vec3(0.f, 0.f, 1.65f) * mManipulator.scale + mManipulator.location);
        }
        
        mManipulator.collisionObjects[i]->setCollisionShape(mManipulator.collisionShapes[i]);
        
        mManipulatorCollisionWorld->addCollisionObject(mManipulator.collisionObjects[i]);
    }
}

void DesignerGameLayer::loadSlimeShader() {
    ResourceManager* resman = ResourceManager::getSingleton();
    
    mSlimeShader.mShaderProg = resman->findShaderProgram("Manipulator.shaderProgram");
    mSlimeShader.mShaderProg->grab();
    
    const std::vector<ShaderProgramResource::Control>& uniformFloats = mSlimeShader.mShaderProg->getUniformVec3s();
    for(std::vector<ShaderProgramResource::Control>::const_iterator iter = uniformFloats.begin(); iter != uniformFloats.end(); ++ iter) {
        const ShaderProgramResource::Control& entry = *iter;
        if(entry.name == "color") {
            mSlimeShader.mColorHandle = entry.handle;
        } else if(entry.name == "sunDirection") {
            mSlimeShader.mSunHandle = entry.handle;
        }
    }
    
    mSlimeShader.mVertexBall = resman->findGeometry("VertexBall.geometry");
    mSlimeShader.mVertexBall->grab();
    mSlimeShader.mStraightEdge = resman->findGeometry("StraightEdge.geometry");
    mSlimeShader.mStraightEdge->grab();
    
    glGenVertexArrays(1, &mSlimeShader.mVertexBallVAO);
    glBindVertexArray(mSlimeShader.mVertexBallVAO);
    mSlimeShader.mVertexBall->bindBuffers();
    if(mSlimeShader.mShaderProg->needsPosAttrib()) {
        mSlimeShader.mVertexBall->enablePositionAttrib(mSlimeShader.mShaderProg->getPosAttrib());
    }
    if(mSlimeShader.mShaderProg->needsNormalAttrib()) {
        mSlimeShader.mVertexBall->enableNormalAttrib(mSlimeShader.mShaderProg->getNormalAttrib());
    }
    glBindVertexArray(0);
    
    glGenVertexArrays(1, &mSlimeShader.mStraightEdgeVAO);
    glBindVertexArray(mSlimeShader.mStraightEdgeVAO);
    mSlimeShader.mStraightEdge->bindBuffers();
    if(mSlimeShader.mShaderProg->needsPosAttrib()) {
        mSlimeShader.mStraightEdge->enablePositionAttrib(mSlimeShader.mShaderProg->getPosAttrib());
    }
    if(mSlimeShader.mShaderProg->needsNormalAttrib()) {
        mSlimeShader.mStraightEdge->enableNormalAttrib(mSlimeShader.mShaderProg->getNormalAttrib());
    }
    glBindVertexArray(0);
    
}

void DesignerGameLayer::loadManipulator() {
    
    for(uint8_t i = 0; i < 6; ++ i) {
        mManipulator.collisionShapes[i] = new btEmptyShape();
        mManipulator.collisionObjects[i] = new btCollisionObject();
        mManipulator.motionStates[i] = new btDefaultMotionState();
        mManipulator.collisionObjects[i]->setCollisionShape(mManipulator.collisionShapes[i]);
        mManipulator.collisionObjects[i]->getWorldTransform().setOrigin(Vec3(0.f, 0.f, 0.f));
        mManipulatorCollisionWorld->addCollisionObject(mManipulator.collisionObjects[i]);
    }
    
    ResourceManager* resman = ResourceManager::getSingleton();
    
    mManipulator.arrow = resman->findGeometry("ManipulatorArrow.geometry");
    mManipulator.arrow->grab();
    mManipulator.wheel = resman->findGeometry("ManipulatorWheel.geometry");
    mManipulator.wheel->grab();
    
    glGenVertexArrays(1, &mManipulator.arrowVAO);
    glBindVertexArray(mManipulator.arrowVAO);
    mManipulator.arrow->bindBuffers();
    if(mSlimeShader.mShaderProg->needsPosAttrib()) {
        mManipulator.arrow->enablePositionAttrib(mSlimeShader.mShaderProg->getPosAttrib());
    }
    if(mSlimeShader.mShaderProg->needsNormalAttrib()) {
        mManipulator.arrow->enableNormalAttrib(mSlimeShader.mShaderProg->getNormalAttrib());
    }
    glBindVertexArray(0);
    
    glGenVertexArrays(1, &mManipulator.wheelVAO);
    glBindVertexArray(mManipulator.wheelVAO);
    mManipulator.wheel->bindBuffers();
    if(mSlimeShader.mShaderProg->needsPosAttrib()) {
        mManipulator.wheel->enablePositionAttrib(mSlimeShader.mShaderProg->getPosAttrib());
    }
    if(mSlimeShader.mShaderProg->needsNormalAttrib()) {
        mManipulator.wheel->enableNormalAttrib(mSlimeShader.mShaderProg->getNormalAttrib());
    }
    glBindVertexArray(0);
}

void DesignerGameLayer::unloadManipulator() {
    glDeleteVertexArrays(1, &mManipulator.arrowVAO);
    glDeleteVertexArrays(1, &mManipulator.wheelVAO);
    mManipulator.arrow->drop();
    mManipulator.wheel->drop();
}

void DesignerGameLayer::unloadSlimeShader() {
    mSlimeShader.mShaderProg->drop();
}

void DesignerGameLayer::onEnd() {
    unloadManipulator();
    unloadSlimeShader();
    
    mRenderer->drop();
    
    mInfCheck->drop();
    mRootNode->drop();
    
    mDebugCube->drop();
}

void DesignerGameLayer::onTick(float tpf, const InputState* inputStates) {
    
    cyclicSawtooth = fmod(cyclicSawtooth + tpf, 1.f);
    cyclicSinusodal = (std::sin(cyclicSawtooth * 6.2831f) + 1.f) * 0.5f;
    
    mMouseLoc = Vec2(((float) inputStates->getMouseX()) / ((float) mScreenWidth), ((float) inputStates->getMouseY()) / ((float) mScreenHeight));
    
    Vec3 movement;
    if(inputStates->isPressed(Input::Scancode::K_D)) {
        movement.x += 1.f;
    }
    if(inputStates->isPressed(Input::Scancode::K_A)) {
        movement.x -= 1.f;
    }
    if(inputStates->isPressed(Input::Scancode::K_S)) {
        movement.z += 1.f;
    }
    if(inputStates->isPressed(Input::Scancode::K_W)) {
        movement.z -= 1.f;
    }
    if(inputStates->isPressed(Input::Scancode::K_E)) {
        movement.y += 1.f;
    }
    if(inputStates->isPressed(Input::Scancode::K_C)) {
        movement.y -= 1.f;
    }
    if(!movement.isZero()) {
        if(mCameraSpeed > mCameraSpeedMax) {
            mCameraSpeed = mCameraSpeedMax;
        }
        movement *= mCameraSpeed;
        mCameraSpeed += mCameraAcceleration * tpf;
        if(inputStates->isPressed(Input::Scancode::K_SHIFT_LEFT)) {
            movement *= 2.f;
        }
        
        movement = glm::vec3(mCamRollNode->calcWorldTransform() * glm::vec4(glm::vec3(movement), 0.f) * tpf);
        
        mCamLocNode->move(movement);
    } else {
        mCameraSpeed = mCameraSpeedMin;
    }
    
    float sensitivity = 1.0;
    if(inputStates->isPressed(Input::Scancode::K_ARROW_UP)) {
        mCamPitchNode->rotatePitch(tpf * sensitivity);
    }
    if(inputStates->isPressed(Input::Scancode::K_ARROW_DOWN)) {
        mCamPitchNode->rotatePitch(-tpf * sensitivity);
    }
    if(inputStates->isPressed(Input::Scancode::K_ARROW_LEFT)) {
        mCamYawNode->rotateYaw(tpf * sensitivity);
    }
    if(inputStates->isPressed(Input::Scancode::K_ARROW_RIGHT)) {
        mCamYawNode->rotateYaw(-tpf * sensitivity);
    }
    

    // mInfCheck->setFocus(comp->mSceneNode->getLocalTranslation());
    
    glm::vec4 debugShow;
    if(inputStates->isPressed(Input::Scancode::K_1)) {
        debugShow.x = 1.f;
    }
    if(inputStates->isPressed(Input::Scancode::K_2)) {
        debugShow.y = 1.f;
    }
    if(inputStates->isPressed(Input::Scancode::K_3)) {
        debugShow.z = 1.f;
    }
    if(inputStates->isPressed(Input::Scancode::K_4)) {
        debugShow.w = 1.f;
    }
    if(inputStates->isPressed(Input::Scancode::K_5)) {
        mDebugWireframe = true;
    }
    if(inputStates->isPressed(Input::Scancode::K_6)) {
        mDebugWireframe = false;
    }
    
    if(inputStates->isPressed(Input::Scancode::K_Q)) {
        mRenderer->setSunDirection(glm::vec3(mCamRollNode->calcWorldTransform() * glm::vec4(0.f, 0.f, -1.f, 0.f)));
    }
    
    mRenderer->setCameraViewMatrix(glm::inverse(mCamRollNode->calcWorldTransform()));
    
    
    updateManipulatorPhysics();
    
    // Although technically incorrect, it is visually better to do mouse-picking after the camera is updated
    
    // Make one of those selection rays
    Vec2 mouseNDCLoc = mMouseLoc * 2.f - 1.f;
    mouseNDCLoc.y = -mouseNDCLoc.y;
    glm::mat4 cameraMatrix = mRenderer->getCameraProjectionMatrix() * mRenderer->getCameraViewMatrix();
    glm::mat4 invCameraMatrix = glm::inverse(cameraMatrix);
    Vec3 mouseRayStart, mouseRayEnd;
    {
        glm::vec4 rayStart = glm::vec4(mouseNDCLoc.x, mouseNDCLoc.y, -1.f, 1.f);
        glm::vec4 rayEnd = glm::vec4(mouseNDCLoc.x, mouseNDCLoc.y, 1.f, 1.f);
        rayStart = invCameraMatrix * rayStart;
        rayEnd = invCameraMatrix * rayEnd;
        mouseRayStart = rayStart / rayStart.w;
        mouseRayEnd = rayEnd / rayEnd.w;
    }
    
    mPlateHovered = nullptr;
    Vec3 hitPoint;
    
    if(!mPlateFreeDragged && mManipulator.handleDragged == -1 && mPlateSelected)
    {
        btCollisionWorld::AllHitsRayResultCallback rayCallback(mouseRayStart, mouseRayEnd);
        mManipulatorCollisionWorld->rayTest(mouseRayStart, mouseRayEnd, rayCallback);
        
        const btCollisionObject* touched = nullptr;
        if(rayCallback.hasHit()) {
            // Cannot rely on the order of rayCallback.m_collisionObjects, so we have to compare the distances manually
            btScalar closestHitFraction(1337); // All fractions are <= 1 so this is effectively infinite
            for(int i = rayCallback.m_collisionObjects.size() - 1; i >= 0; -- i) {
                if(rayCallback.m_hitFractions.at(i) <= closestHitFraction) {
                    touched = rayCallback.m_collisionObjects.at(i);
                    closestHitFraction = rayCallback.m_hitFractions.at(i);
                    hitPoint = rayCallback.m_hitPointWorld.at(i);
                }
            }
        }
        
        mManipulator.handleHovered = -1;
        if(touched) {
            for(int8_t i = 0; i < 6; ++ i) {
                if(mManipulator.collisionObjects[i] == touched) {
                    mManipulator.handleHovered = i;
                    break;
                }
            }
        }
    }
    
    if(!mPlateFreeDragged && mManipulator.handleDragged == -1 && mManipulator.handleHovered == -1) {
        
        btCollisionWorld::AllHitsRayResultCallback rayCallback(mouseRayStart, mouseRayEnd);
        mCollisionWorld->rayTest(mouseRayStart, mouseRayEnd, rayCallback);

        //
        if(rayCallback.hasHit()) {
            // Cannot rely on the order of rayCallback.m_collisionObjects, so we have to compare the distances manually
            btScalar closestHitFraction(1337); // All fractions are <= 1 so this is effectively infinite
            for(int i = rayCallback.m_collisionObjects.size() - 1; i >= 0; -- i) {
                if(rayCallback.m_hitFractions.at(i) <= closestHitFraction) {
                    const btCollisionObject* other = rayCallback.m_collisionObjects.at(i);
                    closestHitFraction = rayCallback.m_hitFractions.at(i);
                    hitPoint = rayCallback.m_hitPointWorld.at(i);
                    mPlateHovered = static_cast<Plate*>(other->getUserPointer());
                }
            }
        }
    }
    
    if(inputStates->isPressed(Input::Scancode::M_LEFT)) {
        // First click...
        if(!mMouseLeftDownLastFrame) {
            // ...on a plate
            if(mPlateHovered) {
                selectPlate(mPlateHovered);
                mPlateFreeDragged = mPlateHovered;
                mPlateDragPoint = hitPoint - mPlateHovered->getLocation();
                
                glm::vec4 asdf = cameraMatrix * glm::vec4(glm::vec3(hitPoint), 1.f);
                asdf /= asdf.w;
                
                mDragPlaneDistance = asdf.z; //plateTouchPoint.dist(mCamLocNode->calcWorldTranslation());
            }
            // ...on a manipulator handle
            else if(mManipulator.handleHovered != -1) {
                // Select that handle for the next frame
                mManipulator.handleDragged = mManipulator.handleHovered;
                
                // Axis
                if(mManipulator.handleDragged < 3) {
                    Vec3 worldA = mPlateSelected->getLocation();
                    Vec3 worldB = worldA;
                    
                    if(mManipulator.handleDragged == 0) {
                        worldB.x += 1.f;
                    } else if(mManipulator.handleDragged == 1) {
                        worldB.y += 1.f;
                    } else {
                        worldB.z += 1.f;
                    }
                    
                    float frac = nearestPointOn3DLine(worldA, worldB, mouseRayStart, mouseRayEnd);
                    if(frac < -100.f) {
                        frac = -100.f;
                    } else if(frac > 100.f) {
                        frac = 100.f;
                    }
                    
                    mManipulator.initialAxisDragFrac = frac;
                }
                
                // Wheel
                else {
                    
                    Vec3 origin = mPlateSelected->getRenderLocation();
                    
                    if(mManipulator.handleDragged == 3) {
                        float divisor = mouseRayEnd.x - mouseRayStart.x;
                        if(divisor == 0.f) { mManipulator.handleDragged = -1; }
                        float fixedX = 1.65f * mManipulator.scale + origin.x;
                        float frac = (fixedX - mouseRayStart.x) / divisor;
                        mManipulator.initialWheelDragVector = Vec2(mouseRayStart.y + (mouseRayEnd.y - mouseRayStart.y) * frac, mouseRayStart.z + (mouseRayEnd.z - mouseRayStart.z) * frac);
                        mManipulator.initialWheelDragVector = mManipulator.initialWheelDragVector - Vec2(origin.y, origin.z);
                    } else if(mManipulator.handleDragged == 4) {
                        float divisor = mouseRayEnd.y - mouseRayStart.y;
                        if(divisor == 0.f) { mManipulator.handleDragged = -1; }
                        float fixedX = 1.65f * mManipulator.scale + origin.y;
                        float frac = (fixedX - mouseRayStart.y) / divisor;
                        mManipulator.initialWheelDragVector = Vec2(mouseRayStart.x + (mouseRayEnd.x - mouseRayStart.x) * frac, mouseRayStart.z + (mouseRayEnd.z - mouseRayStart.z) * frac);
                        mManipulator.initialWheelDragVector = mManipulator.initialWheelDragVector - Vec2(origin.x, origin.z);
                    } else {
                        float divisor = mouseRayEnd.z - mouseRayStart.z;
                        if(divisor == 0.f) { mManipulator.handleDragged = -1; }
                        float fixedX = 1.65f * mManipulator.scale + origin.z;
                        float frac = (fixedX - mouseRayStart.z) / divisor;
                        mManipulator.initialWheelDragVector = Vec2(mouseRayStart.x + (mouseRayEnd.x - mouseRayStart.x) * frac, mouseRayStart.y + (mouseRayEnd.y - mouseRayStart.y) * frac);
                        mManipulator.initialWheelDragVector = mManipulator.initialWheelDragVector - Vec2(origin.x, origin.y);
                    }
                }
            }
            // ...on nothing
            else {
                deselectPlate();
                mManipulator.handleDragged = -1;
            }
        }
        // Being held down...
        else {
            // ... with a plate in free drag mode
            if(mPlateFreeDragged) {
                float correctedZ = mDragPlaneDistance; //((1.f / mDragPlaneDistance) - (1.f / near)) / ((1.f / far) - (1.f / near)) * 2.f - 1.f;
                
                glm::vec4 worldSpaceDragSpot = glm::vec4(mouseNDCLoc.x, mouseNDCLoc.y, correctedZ, 1.f);
                worldSpaceDragSpot = invCameraMatrix * worldSpaceDragSpot;
                worldSpaceDragSpot /= worldSpaceDragSpot.w;
                
                Vec3 targetLoc(worldSpaceDragSpot);
                
                targetLoc -= mPlateDragPoint;
                
                mPlateFreeDragged->setLocation(targetLoc, 1.f / ((float) mGridSize));
            }
            // ... with a handle being dragged
            else if(mManipulator.handleDragged != -1) {
                
                // arrow
                if(mManipulator.handleDragged < 3) {
                    Vec3 worldA = mPlateSelected->getLocation();
                    Vec3 worldB = worldA;
                    
                    if(mManipulator.handleDragged == 0) {
                        worldB.x += 1.f;
                    } else if(mManipulator.handleDragged == 1) {
                        worldB.y += 1.f;
                    } else {
                        worldB.z += 1.f;
                    }
                    
                    float frac = nearestPointOn3DLine(worldA, worldB, mouseRayStart, mouseRayEnd);
                    if(frac < -100.f) {
                        frac = -100.f;
                    } else if(frac > 100.f) {
                        frac = 100.f;
                    }
                    
                    frac -= mManipulator.initialAxisDragFrac;
                    
                    Vec3 targetLoc = (worldB - worldA) * frac + worldA;
                    
                    mPlateSelected->setLocation(targetLoc, 1.f / ((float) mGridSize));
                }
                
                // wheel
                else {
                    Vec2 dragSpot;
                    bool isError = false;
                    
                    Vec3 origin = mPlateSelected->getRenderLocation();
                    
                    if(mManipulator.handleDragged == 3) {
                        float divisor = mouseRayEnd.x - mouseRayStart.x;
                        if(divisor == 0.f) { isError = true; } else {
                            float fixedX = 1.65f * mManipulator.scale + origin.x;
                            float frac = (fixedX - mouseRayStart.x) / divisor;
                            dragSpot = Vec2(mouseRayStart.y + (mouseRayEnd.y - mouseRayStart.y) * frac, mouseRayStart.z + (mouseRayEnd.z - mouseRayStart.z) * frac);
                            dragSpot = dragSpot - Vec2(origin.y, origin.z);
                        }
                    } else if(mManipulator.handleDragged == 4) {
                        float divisor = mouseRayEnd.y - mouseRayStart.y;
                        if(divisor == 0.f) { isError = true; } else {
                            float fixedX = 1.65f * mManipulator.scale + origin.y;
                            float frac = (fixedX - mouseRayStart.y) / divisor;
                            dragSpot = Vec2(mouseRayStart.x + (mouseRayEnd.x - mouseRayStart.x) * frac, mouseRayStart.z + (mouseRayEnd.z - mouseRayStart.z) * frac);
                            dragSpot = dragSpot - Vec2(origin.x, origin.z);
                        }
                    } else {
                        float divisor = mouseRayEnd.z - mouseRayStart.z;
                        if(divisor == 0.f) { isError = true; } else {
                            float fixedX = 1.65f * mManipulator.scale + origin.z;
                            float frac = (fixedX - mouseRayStart.z) / divisor;
                            dragSpot = Vec2(mouseRayStart.x + (mouseRayEnd.x - mouseRayStart.x) * frac, mouseRayStart.y + (mouseRayEnd.y - mouseRayStart.y) * frac);
                            dragSpot = dragSpot - Vec2(origin.x, origin.y);
                        }
                    }
                    
                    if(!isError) {
                        float angle = std::atan2( 
                        dragSpot.determinant(mManipulator.initialWheelDragVector),
                        dragSpot.dot(mManipulator.initialWheelDragVector));
                        
                        
                        if(mManipulator.handleDragged == 3) {
                            angle = -angle;
                            mPlateSelected->setIntermediatePitch(angle);
                        } else if(mManipulator.handleDragged == 4) {
                            mPlateSelected->setIntermediateYaw(angle);
                        } else {
                            angle = -angle;
                            mPlateSelected->setIntermediateRoll(angle);
                        }
                    }
                }
            }
        }
        mMouseLeftDownLastFrame = true;
    }
    else {
        mPlateFreeDragged = nullptr;
        mMouseLeftDownLastFrame = false;
        mManipulator.handleDragged = -1;
        if(mPlateSelected) {
            mPlateSelected->finalizeRotation();
        }
    }
    
    for(std::vector<Plate*>::iterator iter = mPlates.begin(); iter != mPlates.end(); ++ iter) {
        Plate* targetLoc = *iter;
        
        targetLoc->tick(tpf);
    }
    
    mRenderer->renderFrame(mRootNode, debugShow, mDebugWireframe);
    
    renderSecondLayer();
    
    if(tpf > 0) {
        float fpsNew = 1 / tpf;
        fps = (fps * fpsWeight) + (fpsNew * (1.f - fpsWeight));
    }
    oneSecondTimer += tpf;
    if(oneSecondTimer > 1.f) {
        oneSecondTimer -= 1.f;
        std::cout << "FPS: " << (uint32_t) fps << std::endl;
    }
}

void DesignerGameLayer::renderSecondLayer() {
    
    glViewport(0, 0, mScreenWidth, mScreenHeight);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glDepthMask(GL_TRUE);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_STENCIL_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_CONSTANT_COLOR, GL_ONE_MINUS_CONSTANT_COLOR);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glClear(GL_DEPTH_BUFFER_BIT);
    
    updateManipulatorTransform();

    float alphaDefault = 0.3f;
    float alphaHover = cyclicSinusodal * 0.5f + 0.3f;
    
    SceneNode* mUtilityNode = new SceneNode();
    mUtilityNode->grab();
    if(mPlateSelected) {
        mPlateSelected->renderEdges(mRenderer, mSlimeShader);
    }
    
    if(mPlateSelected && !mPlateFreeDragged) {
        mUtilityNode->setLocalTranslation(mManipulator.location);
        mUtilityNode->setLocalScale(Vec3(mManipulator.scale));
        mUtilityNode->rotateYaw(glm::radians(90.f));

        glUseProgram(mSlimeShader.mShaderProg->getHandle());
        
        glUniform3fv(mSlimeShader.mSunHandle, 1, glm::value_ptr(mRenderer->getSunDirection() * -1.f));
        
        for(int8_t i = 0; i < 3; ++ i) {
            if(i == 0) {
                glUniform3fv(mSlimeShader.mColorHandle, 1, glm::value_ptr(glm::vec3(1.f, 0.f, 0.f)));
            }
            else if(i == 1) {
                glUniform3fv(mSlimeShader.mColorHandle, 1, glm::value_ptr(glm::vec3(0.f, 1.f, 0.f)));
                mUtilityNode->rotateYaw(glm::radians(-90.f));
                mUtilityNode->rotatePitch(glm::radians(-90.f));
            }
            else if(i == 2) {
                glUniform3fv(mSlimeShader.mColorHandle, 1, glm::value_ptr(glm::vec3(0.f, 0.f, 1.f)));
                mUtilityNode->rotatePitch(glm::radians(90.f));
            }
            
            mSlimeShader.mShaderProg->bindModelViewProjMatrices(mUtilityNode->calcLocalTransform(), mRenderer->getCameraViewMatrix(), mRenderer->getCameraProjectionMatrix());
            
            glBindVertexArray(mManipulator.arrowVAO);
            if(mManipulator.handleHovered == i) {
                glBlendColor(alphaHover, alphaHover, alphaHover, 1.0f);
            } else {
                glBlendColor(alphaDefault, alphaDefault, alphaDefault, 1.0f);
            }
            if(mManipulator.handleDragged == i) {
                glDisable(GL_BLEND);
            } else {
                glEnable(GL_BLEND);
            }
            mManipulator.arrow->drawElements();
            
            glBindVertexArray(mManipulator.wheelVAO);
            if(mManipulator.handleHovered == i + 3) {
                glBlendColor(alphaHover, alphaHover, alphaHover, 1.0f);
            } else {
                glBlendColor(alphaDefault, alphaDefault, alphaDefault, 1.0f);
            }
            if(mManipulator.handleDragged == i + 3) {
                glDisable(GL_BLEND);
            } else {
                glEnable(GL_BLEND);
            }
            mManipulator.wheel->drawElements();
        }
        
        
        glBindVertexArray(0);
        glUseProgram(0);
    }
    
    mUtilityNode->drop();
}

void DesignerGameLayer::Plate::renderEdges(const DeferredRenderer* mRenderer, const SlimeShader& mSlimeShader) const {
    for(std::vector<Edge*>::const_iterator iter = mEdges.cbegin(); iter != mEdges.cend(); ++ iter) {
        const Edge* edge = *iter;
        edge->render(mRenderer, mSlimeShader);
        break;
    }
}

void DesignerGameLayer::StraightEdge::render(const DeferredRenderer* mRenderer, const SlimeShader& mSlimeShader) const {
    
    Vec3 disp = mEndLoc - mStartLoc;
    Vec3 dir = disp.normalized();
    
    
    
    SceneNode* mUtilityNode = new SceneNode();
    mUtilityNode->grab();
    mUtilityNode->setLocalOrientation(quaternionLookAt(dir, Vec3(0.f, 0.f, 1.f), Vec3(0.f, 1.f, 0.f)));
    
    glUseProgram(mSlimeShader.mShaderProg->getHandle());
    
    glUniform3fv(mSlimeShader.mSunHandle, 1, glm::value_ptr(mRenderer->getSunDirection() * -1.f));
    glUniform3fv(mSlimeShader.mColorHandle, 1, glm::value_ptr(glm::vec3(1.f, 1.f, 0.f)));
    mSlimeShader.mShaderProg->bindModelViewProjMatrices(mUtilityNode->calcWorldTransform(), mRenderer->getCameraViewMatrix(), mRenderer->getCameraProjectionMatrix());
    glBindVertexArray(mSlimeShader.mStraightEdgeVAO);
    mSlimeShader.mStraightEdge->drawElements();
    
    
    glBindVertexArray(0);
    glUseProgram(0);
    
    mUtilityNode->drop();
    
}

bool DesignerGameLayer::onWindowSizeUpdate(const WindowResizeEvent& event) {
    return true;
}
}

