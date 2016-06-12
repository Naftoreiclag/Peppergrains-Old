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
#include <iostream>
#include <sstream>

#include "glm/gtx/string_cast.hpp"
#include "SDL2/SDL.h"

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

DesignerGameLayer::Plate::Plate()
: integralX(0)
, integralY(0)
, integralZ(0) { }
DesignerGameLayer::Plate::~Plate() { }

Vec3 DesignerGameLayer::Plate::getLocation() const {
    return Vec3(
        ((float) integralX) * (1.f / 60.f),
        ((float) integralY) * (1.f / 60.f),
        ((float) integralZ) * (1.f / 60.f)
    );
}

void DesignerGameLayer::Plate::setLocation(Vec3 location) {
    integralX = (uint32_t) std::floor(location.x * 60.f + 0.5);
    integralY = (uint32_t) std::floor(location.y * 60.f + 0.5);
    integralZ = (uint32_t) std::floor(location.z * 60.f + 0.5);
}

void DesignerGameLayer::Plate::tick(float tpf) {
    Vec3 target((float) integralX, (float) integralY, (float) integralZ);
    target /= 60.f;
    
    /*
    // Exponential decay
    if((target - renderLocation).magSq() < 0.001f) {
        renderLocation = target;
    }
    else {
        renderLocation += (target - renderLocation) * std::pow(0.5f, 1.f / tpf);
    }
    */
    
    renderLocation = target;
    
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

    mBroadphase = new btDbvtBroadphase();
    mCollisionConfiguration = new btDefaultCollisionConfiguration();
    mDispatcher = new btCollisionDispatcher(mCollisionConfiguration);
    mCollisionWorld = new btCollisionWorld(mDispatcher, mBroadphase, mCollisionConfiguration);
    
    
    mDebugCube = mRootNode->newChild();
    mDebugCube->grabModel(resman->findModel("RoseCube.model"));
    mDebugCube->setLocalScale(Vec3(0.2, 0.2, 0.2));
    mDebugCube->grab();
    mDebugCube->setLocalTranslation(Vec3(999, 999, 999));
    
    
    mInfCheck = new InfiniteCheckerboardModel();
    mInfCheck->grab();
    mRootNode->newChild()->grabModel(mInfCheck);

    fps = 0.f;
    fpsWeight = 0.85f;
    
    mDebugWireframe = false;

    oneSecondTimer = 0.f;
    
    mPlateHighlighted = nullptr;
    
    mGridSize = 60;
    
    newPlate();
}
void DesignerGameLayer::onEnd() {
    mRenderer->drop();
    
    mInfCheck->drop();
    mRootNode->drop();
    
    mDebugCube->drop();
    
    delete mCollisionWorld;
    delete mDispatcher;
    delete mCollisionConfiguration;
    delete mBroadphase;
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
    
    btCollisionWorld::AllHitsRayResultCallback rayCallback(absStart, absEnd);
    mCollisionWorld->rayTest(absStart, absEnd, rayCallback);

    //
    Plate* plateUnderCursor = nullptr;
    Vec3 plateTouchPoint;
    float hitFraction;
    if(rayCallback.hasHit()) {
        // Cannot rely on the order of rayCallback.m_collisionObjects, so we have to compare the distances manually
        btScalar closestHitFraction(1337); // All fractions are <= 1 so this is effectively infinite
        for(int i = rayCallback.m_collisionObjects.size() - 1; i >= 0; -- i) {
            if(rayCallback.m_hitFractions.at(i) <= closestHitFraction) {
                const btCollisionObject* other = rayCallback.m_collisionObjects.at(i);
                closestHitFraction = rayCallback.m_hitFractions.at(i);
                plateTouchPoint = rayCallback.m_hitPointWorld.at(i);
                plateUnderCursor = static_cast<Plate*>(other->getUserPointer());
                // btRigidBody* groundBody = static_cast<btRigidBody*>(other);
            }
        }
        hitFraction = closestHitFraction;
    }
    
    if(inputStates->isPressed(Input::Scancode::M_LEFT)) {
        if(mPlateHighlighted) {
            
            const float& near = mRenderer->getCameraNearDepth();
            const float& far = mRenderer->getCameraFarDepth();
            float correctedZ = mDragPlaneDistance; //((1.f / mDragPlaneDistance) - (1.f / near)) / ((1.f / far) - (1.f / near)) * 2.f - 1.f;
            
            glm::vec4 worldSpaceDragSpot = glm::vec4(ndcMouse.x, ndcMouse.y, correctedZ, 1.f);
            worldSpaceDragSpot = invCameraMatrix * worldSpaceDragSpot;
            worldSpaceDragSpot /= worldSpaceDragSpot.w;
            
            Vec3 potato(worldSpaceDragSpot);
            
            std::cout << potato << std::endl;
            
            potato -= mPlateDragPoint;
            
            
            mPlateHighlighted->setLocation(potato);
            
        }
        else {
            if(plateUnderCursor) {
                mPlateHighlighted = plateUnderCursor;
                mPlateDragPoint = plateTouchPoint - plateUnderCursor->getLocation();
                
                glm::vec4 asdf = cameraMatrix * glm::vec4(glm::vec3(plateTouchPoint), 1.f);
                asdf /= asdf.w;
                
                
                mDragPlaneDistance = asdf.z; //plateTouchPoint.dist(mCamLocNode->calcWorldTranslation());
                std::cout << "Selected" << std::endl;
                std::cout << mPlateDragPoint << std::endl;
            }
        }
    }
    else {
        mPlateHighlighted = nullptr;
    }
    if(plateUnderCursor) {
        mDebugCube->setLocalTranslation(plateTouchPoint);
    }
    
    for(std::vector<Plate*>::iterator iter = mPlates.begin(); iter != mPlates.end(); ++ iter) {
        Plate* potato = *iter;
        
        potato->tick(tpf);
    }
    
    mRenderer->renderFrame(mRootNode, debugShow, mDebugWireframe);
    
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

