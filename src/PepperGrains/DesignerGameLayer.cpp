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
    
float toNearestMultiple(const float& a, const float& b) {
    return std::floor((a / b) + 0.5) * b;
}

float cotangent(const float& a) {
    return std::cos(a) / std::sin(a);
}

DesignerGameLayer::Manipulator::Manipulator(){
}
DesignerGameLayer::Manipulator::~Manipulator() {
    
}
    
DesignerGameLayer::Plate::Plate()
: integralX(0)
, integralY(0)
, integralZ(0) { }
DesignerGameLayer::Plate::~Plate() { }

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
        ((float) integralX) * (1.f / 60.f),
        ((float) integralY) * (1.f / 60.f),
        ((float) integralZ) * (1.f / 60.f)
    );
}

Vec3 DesignerGameLayer::Plate::getRenderLocation() const {
    return renderLocation;
}

void DesignerGameLayer::Plate::setLocation(Vec3 location, float snapSize) {
    integralX = (uint32_t) std::floor(toNearestMultiple(location.x, snapSize) * 60.f + 0.5);
    integralY = (uint32_t) std::floor(toNearestMultiple(location.y, snapSize) * 60.f + 0.5);
    integralZ = (uint32_t) std::floor(toNearestMultiple(location.z, snapSize) * 60.f + 0.5);
}

void DesignerGameLayer::Plate::tick(float tpf) {
    Vec3 target((float) integralX, (float) integralY, (float) integralZ);
    target /= 60.f;
    
    // Exponential decay
    if((target - renderLocation).mag() < tpf) {
        renderLocation = target;
    }
    else {
        renderLocation += (target - renderLocation) * tpf * 25.f;
    }
    
    collisionObject->getWorldTransform().setOrigin(renderLocation);
    collisionWorld->removeCollisionObject(collisionObject);
    collisionWorld->addCollisionObject(collisionObject);
    
    sceneNode->setLocalTranslation(renderLocation);
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
    
    plate->sceneNode = mRootNode->newChild();
    plate->sceneNode->grab();
    plate->sceneNode->grabModel(resman->findModel("SquarePlate.model"));
    
    plate->collisionShape = new btBoxShape(Vec3(0.5f, 1.f / 60.f, 0.5f));
    plate->motionState = new btDefaultMotionState();
    plate->collisionObject = new btCollisionObject();
    plate->collisionObject->setCollisionShape(plate->collisionShape);
    plate->collisionObject->setUserPointer(plate);
    plate->collisionObject->getWorldTransform().setOrigin(Vec3(0.f, 0.f, 0.f));
    
    mCollisionWorld->addCollisionObject(plate->collisionObject);
    plate->collisionWorld = mCollisionWorld;
    
    mPlates.push_back(plate);
}

void DesignerGameLayer::deletePlate(Plate* plate) {
    mPlates.erase(std::remove(mPlates.begin(), mPlates.end(), plate), mPlates.end());
    
    plate->sceneNode->drop();
    
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
    
    mUtilityNode = new SceneNode();
    mUtilityNode->grab();
    
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
    
    loadManipulator();
    
    mInfCheck = new InfiniteCheckerboardModel();
    mInfCheck->grab();
    mRootNode->newChild()->grabModel(mInfCheck);

    fps = 0.f;
    fpsWeight = 0.85f;
    
    mDebugWireframe = false;

    oneSecondTimer = 0.f;
    
    mPlateDragged = nullptr;
    mPlateSelected = nullptr;
    
    mGridSize = 2;
    
    newPlate();
    newPlate();
    newPlate();
}

void DesignerGameLayer::updateManipulatorTransform() {
    if(!mPlateSelected) {
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
    mManipulator.location = mPlateSelected->getRenderLocation();
}

void DesignerGameLayer::updateManipulatorPhysics() {
    updateManipulatorTransform();
    if(!mPlateSelected) {
        return;
    }
    
    for(uint8_t i = 0; i < 6; ++ i) {
        mManipulatorCollisionWorld->removeCollisionObject(mManipulator.collisionObjects[i]);
        
        delete mManipulator.collisionShapes[i];
        
        if(i == 0) {
            mManipulator.collisionShapes[i] = new btCylinderShapeX(Vec3(0.625f, 0.05f, 0.05f) * mManipulator.scale);
            mManipulator.collisionObjects[i]->getWorldTransform().setOrigin(Vec3(0.775f, 0.f, 0.f) + mManipulator.location);
        } else if(i == 1) {
            mManipulator.collisionShapes[i] = new btCylinderShape(Vec3(0.05f, 0.625f, 0.05f) * mManipulator.scale);
            mManipulator.collisionObjects[i]->getWorldTransform().setOrigin(Vec3(0.f, 0.775f, 0.f) + mManipulator.location);
        } else if(i == 2) {
            mManipulator.collisionShapes[i] = new btCylinderShapeZ(Vec3(0.05f, 0.05f, 0.625f) * mManipulator.scale);
            mManipulator.collisionObjects[i]->getWorldTransform().setOrigin(Vec3(0.f, 0.f, 0.775f) + mManipulator.location);
        } else if(i == 3) {
            mManipulator.collisionShapes[i] = new btCylinderShapeX(Vec3(0.05f, 0.3f, 0.3f) * mManipulator.scale);
            mManipulator.collisionObjects[i]->getWorldTransform().setOrigin(Vec3(1.65f, 0.f, 0.f) + mManipulator.location);
        } else if(i == 4) {
            mManipulator.collisionShapes[i] = new btCylinderShape(Vec3(0.3f, 0.05f, 0.3f) * mManipulator.scale);
            mManipulator.collisionObjects[i]->getWorldTransform().setOrigin(Vec3(0.f, 1.65f, 0.f) + mManipulator.location);
        } else if(i == 5) {
            mManipulator.collisionShapes[i] = new btCylinderShapeZ(Vec3(0.3f, 0.3f, 0.05f) * mManipulator.scale);
            mManipulator.collisionObjects[i]->getWorldTransform().setOrigin(Vec3(0.f, 0.f, 1.65f) + mManipulator.location);
        }
        
        mManipulator.collisionObjects[i]->setCollisionShape(mManipulator.collisionShapes[i]);
        
        mManipulatorCollisionWorld->addCollisionObject(mManipulator.collisionObjects[i]);
    }
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
    mManipulator.shaderProg = resman->findShaderProgram("Manipulator.shaderProgram");
    mManipulator.shaderProg->grab();
    
    const std::vector<ShaderProgramResource::Control>& uniformFloats = mManipulator.shaderProg->getUniformVec3s();
    for(std::vector<ShaderProgramResource::Control>::const_iterator iter = uniformFloats.begin(); iter != uniformFloats.end(); ++ iter) {
        const ShaderProgramResource::Control& entry = *iter;
        if(entry.name == "color") {
            mManipulator.colorHandle = entry.handle;
        } else if(entry.name == "sunDirection") {
            mManipulator.sunHandle = entry.handle;
        }
    }
    
    glGenVertexArrays(1, &mManipulator.arrowVAO);
    glBindVertexArray(mManipulator.arrowVAO);
    mManipulator.arrow->bindBuffers();
    if(mManipulator.shaderProg->needsPosAttrib()) {
        mManipulator.arrow->enablePositionAttrib(mManipulator.shaderProg->getPosAttrib());
    }
    if(mManipulator.shaderProg->needsNormalAttrib()) {
        mManipulator.arrow->enableNormalAttrib(mManipulator.shaderProg->getNormalAttrib());
    }
    glBindVertexArray(0);
    
    glGenVertexArrays(1, &mManipulator.wheelVAO);
    glBindVertexArray(mManipulator.wheelVAO);
    mManipulator.wheel->bindBuffers();
    if(mManipulator.shaderProg->needsPosAttrib()) {
        mManipulator.wheel->enablePositionAttrib(mManipulator.shaderProg->getPosAttrib());
    }
    if(mManipulator.shaderProg->needsNormalAttrib()) {
        mManipulator.wheel->enableNormalAttrib(mManipulator.shaderProg->getNormalAttrib());
    }
    glBindVertexArray(0);
}

void DesignerGameLayer::unloadManipulator() {
    glDeleteVertexArrays(1, &mManipulator.arrowVAO);
    glDeleteVertexArrays(1, &mManipulator.wheelVAO);
    mManipulator.arrow->drop();
    mManipulator.wheel->drop();
    mManipulator.shaderProg->drop();
}

void DesignerGameLayer::onEnd() {
    unloadManipulator();
    
    mRenderer->drop();
    
    mInfCheck->drop();
    mRootNode->drop();
    mUtilityNode->drop();
    
    mDebugCube->drop();
}

void DesignerGameLayer::onTick(float tpf, const InputState* inputStates) {
    
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
    Vec2 ndcMouse = mMouseLoc * 2.f - 1.f;
    ndcMouse.y = -ndcMouse.y;
    glm::vec4 rayStart = glm::vec4(ndcMouse.x, ndcMouse.y, -1.f, 1.f);
    glm::vec4 rayEnd = glm::vec4(ndcMouse.x, ndcMouse.y, 1.f, 1.f);
    glm::mat4 cameraMatrix = mRenderer->getCameraProjectionMatrix() * mRenderer->getCameraViewMatrix();
    glm::mat4 invCameraMatrix = glm::inverse(cameraMatrix);
    rayStart = invCameraMatrix * rayStart;
    rayEnd = invCameraMatrix * rayEnd;
    rayStart /= rayStart.w;
    rayEnd /= rayEnd.w;
    
    Vec3 absStart = Vec3(rayStart);
    Vec3 absEnd = Vec3(rayEnd);
    
    Plate* plateUnderCursor = nullptr;
    Vec3 hitPoint;
    
    // Search in the manipulator layer first
    {
        btCollisionWorld::AllHitsRayResultCallback rayCallback(absStart, absEnd);
        mManipulatorCollisionWorld->rayTest(absStart, absEnd, rayCallback);
        
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
        
        mManipulator.draggedHandle = -1;
        if(touched) {
            for(uint8_t i = 0; i < 8; ++ i) {
                if(mManipulator.collisionObjects[i] == touched) {
                    mManipulator.draggedHandle = i;
                }
            }
        }
    }
    
    btCollisionWorld::AllHitsRayResultCallback rayCallback(absStart, absEnd);
    mCollisionWorld->rayTest(absStart, absEnd, rayCallback);

    //
    if(rayCallback.hasHit()) {
        // Cannot rely on the order of rayCallback.m_collisionObjects, so we have to compare the distances manually
        btScalar closestHitFraction(1337); // All fractions are <= 1 so this is effectively infinite
        for(int i = rayCallback.m_collisionObjects.size() - 1; i >= 0; -- i) {
            if(rayCallback.m_hitFractions.at(i) <= closestHitFraction) {
                const btCollisionObject* other = rayCallback.m_collisionObjects.at(i);
                closestHitFraction = rayCallback.m_hitFractions.at(i);
                hitPoint = rayCallback.m_hitPointWorld.at(i);
                plateUnderCursor = static_cast<Plate*>(other->getUserPointer());
            }
        }
    }
    
    if(inputStates->isPressed(Input::Scancode::M_LEFT)) {
        if(mPlateDragged) {
            
            float correctedZ = mDragPlaneDistance; //((1.f / mDragPlaneDistance) - (1.f / near)) / ((1.f / far) - (1.f / near)) * 2.f - 1.f;
            
            glm::vec4 worldSpaceDragSpot = glm::vec4(ndcMouse.x, ndcMouse.y, correctedZ, 1.f);
            worldSpaceDragSpot = invCameraMatrix * worldSpaceDragSpot;
            worldSpaceDragSpot /= worldSpaceDragSpot.w;
            
            Vec3 potato(worldSpaceDragSpot);
            
            potato -= mPlateDragPoint;
            
            
            mPlateDragged->setLocation(potato, 1.f / ((float) mGridSize));
            
        }
        else {
            if(plateUnderCursor) {
                selectPlate(plateUnderCursor);
                mPlateDragged = plateUnderCursor;
                mPlateDragPoint = hitPoint - plateUnderCursor->getLocation();
                
                glm::vec4 asdf = cameraMatrix * glm::vec4(glm::vec3(hitPoint), 1.f);
                asdf /= asdf.w;
                
                
                mDragPlaneDistance = asdf.z; //plateTouchPoint.dist(mCamLocNode->calcWorldTranslation());
            }
            else {
                deselectPlate();
            }
        }
    }
    else {
        mPlateDragged = nullptr;
    }
    if(plateUnderCursor) {
        //mDebugCube->setLocalTranslation(plateTouchPoint);
    }
    
    
    
    /*
    if(mPlateSelected) {
        glm::vec4 asdf = cameraMatrix * glm::vec4(glm::vec3(mPlateSelected->getLocation()), 1.f);
        asdf /= asdf.w;
        
        float z = asdf.z * 2.f - 1.f;
        const float& near = mRenderer->getCameraNearDepth();
        const float& far = mRenderer->getCameraFarDepth();
        float linDepth = (2.f * near * far) / (far + near - z * (far - near));
        
        float clipRad = cotangent(mRenderer->getCameraFOV() / 2.f) / linDepth;
        
        mDebugCube->setLocalTranslation(mPlateSelected->getLocation());
        mDebugCube->setLocalScale(Vec3(1.f / clipRad));
    }
    else {
        mDebugCube->setLocalScale(Vec3(0.f));
    }
    */
    
    for(std::vector<Plate*>::iterator iter = mPlates.begin(); iter != mPlates.end(); ++ iter) {
        Plate* potato = *iter;
        
        potato->tick(tpf);
    }
    
    mRenderer->renderFrame(mRootNode, debugShow, mDebugWireframe);
    
    renderManipulator();
    
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

void DesignerGameLayer::renderManipulator() {
    
    glViewport(0, 0, mScreenWidth, mScreenHeight);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glDepthMask(GL_TRUE);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_STENCIL_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendColor(0.5f, 0.5f, 0.5f, 1.0f);
    glBlendFunc(GL_CONSTANT_COLOR, GL_ONE_MINUS_CONSTANT_COLOR);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glClear(GL_DEPTH_BUFFER_BIT);
    
    if(!mPlateSelected) {
        return;
    }
    
    updateManipulatorTransform();

    mUtilityNode->resetLocalTransform();
    mUtilityNode->setLocalTranslation(mManipulator.location);
    mUtilityNode->setLocalScale(Vec3(mManipulator.scale));

    glUseProgram(mManipulator.shaderProg->getHandle());
    
    glUniform3fv(mManipulator.sunHandle, 1, glm::value_ptr(mRenderer->getSunDirection() * -1.f));
    
    for(int8_t i = 0; i < 3; ++ i) {
        if(i == 0) {
            glUniform3fv(mManipulator.colorHandle, 1, glm::value_ptr(glm::vec3(0.f, 0.f, 1.f)));
        }
        else if(i == 1) {
            glUniform3fv(mManipulator.colorHandle, 1, glm::value_ptr(glm::vec3(1.f, 0.f, 0.f)));
            mUtilityNode->rotateYaw(glm::radians(90.f));
        }
        else if(i == 2) {
            glUniform3fv(mManipulator.colorHandle, 1, glm::value_ptr(glm::vec3(0.f, 1.f, 0.f)));
            mUtilityNode->rotatePitch(glm::radians(-90.f));
        }
        
        mManipulator.shaderProg->bindModelViewProjMatrices(mUtilityNode->calcLocalTransform(), mRenderer->getCameraViewMatrix(), mRenderer->getCameraProjectionMatrix());
        glBindVertexArray(mManipulator.arrowVAO);
        if(mManipulator.draggedHandle == i) {
            glDisable(GL_BLEND);
        }
        mManipulator.arrow->drawElements();
        glEnable(GL_BLEND);
        if(mManipulator.draggedHandle == i + 3) {
            glDisable(GL_BLEND);
        }
        glBindVertexArray(mManipulator.wheelVAO);
        glEnable(GL_BLEND);
        mManipulator.wheel->drawElements();
    }
    
    
    glBindVertexArray(0);
    glUseProgram(0);
}

bool DesignerGameLayer::onMouseMove(const MouseMoveEvent& event) {
    float x = event.x;
    float y = event.y;
    float dx = event.dx;
    float dy = event.dx;
    
    /*
    mCamYawNode->rotateYaw(-dx * 0.003f);
    mCamPitchNode->rotatePitch(-dy * 0.003f);
    */
    
    mMouseLoc = Vec2(x / ((float) mScreenWidth), y / ((float) mScreenHeight));
    
    return true;
}

bool DesignerGameLayer::onWindowSizeUpdate(const WindowResizeEvent& event) {
    return true;
}
}

