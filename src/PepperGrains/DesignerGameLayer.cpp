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

DesignerGameLayer::DesignerGameLayer(uint32_t width, uint32_t height)
: mScreenWidth(width)
, mScreenHeight(height) {
}

DesignerGameLayer::~DesignerGameLayer() {
}

// Lifecycle
void DesignerGameLayer::onBegin() {
    SDL_SetRelativeMouseMode(SDL_FALSE);
    
    ResourceManager* resman = ResourceManager::getSingleton();
    
    mRenderer = new DeferredRenderer(mScreenWidth, mScreenHeight);
    mRenderer->grab();

    mBroadphase = new btDbvtBroadphase();
    mCollisionConfiguration = new btDefaultCollisionConfiguration();
    mDispatcher = new btCollisionDispatcher(mCollisionConfiguration);
    mSolver = new btSequentialImpulseConstraintSolver();
    mDynamicsWorld = new btDiscreteDynamicsWorld(mDispatcher, mBroadphase, mSolver, mCollisionConfiguration);
    mDynamicsWorld->setGravity(btVector3(0, -9.8, 0));
    
    mRootNode = new SceneNode();
    mRootNode->grab();
    
    mDebugCube = mRootNode->newChild();
    mDebugCube->grabModel(resman->findModel("RoseCube.model"));
    mDebugCube->setLocalScale(Vec3(0.2, 0.2, 0.2));
    mDebugCube->grab();
    mDebugCube->setLocalTranslation(Vec3(999, 999, 999));
    
    mCamLocNode = new SceneNode();
    mCamLocNode->move(Vec3(0.f, 1.5f, 0.f));
    
    mCamRollNode = new SceneNode();
    mCamPitchNode = new SceneNode();
    mCamYawNode = new SceneNode();
    mCamLocNode->addChild(mCamYawNode);
    mCamYawNode->addChild(mCamPitchNode);
    mCamPitchNode->addChild(mCamRollNode);
    
    mInfCheck = new InfiniteCheckerboardModel();
    mInfCheck->grab();
    mRootNode->newChild()->grabModel(mInfCheck);

    fps = 0.f;
    fpsWeight = 0.85f;
    
    mDebugWireframe = false;

    oneSecondTimer = 0.f;
}
void DesignerGameLayer::onEnd() {
    mRenderer->drop();
    
    mInfCheck->drop();
    mRootNode->drop();
    
    mDebugCube->drop();
    
    delete mDynamicsWorld;
    delete mSolver;
    delete mDispatcher;
    delete mCollisionConfiguration;
    delete mBroadphase;
}

void DesignerGameLayer::onTick(float tpf, const InputState* keyStates) {
    
    glm::vec3 movement;
    if(keyStates->isPressed(Input::Scancode::K_W)) {
        movement.z -= 1.0;
    }
    if(keyStates->isPressed(Input::Scancode::K_A)) {
        movement.x -= 1.0;
    }
    if(keyStates->isPressed(Input::Scancode::K_S)) {
        movement.z += 1.0;
    }
    if(keyStates->isPressed(Input::Scancode::K_D)) {
        movement.x += 1.0;
    }
    if(movement != glm::vec3(0.f)) {
        glm::normalize(movement);
        if(keyStates->isPressed(Input::Scancode::K_SHIFT_LEFT)) {
            movement *= 10.f;
        }
        
        movement = glm::vec3(mCamRollNode->calcWorldTransform() * glm::vec4(movement, 0.f) * tpf);
        //
    }
    
    if(keyStates->isPressed(Input::Scancode::M_LEFT)) {
        std::cout << "Left" << std::endl;
    }

    // mInfCheck->setFocus(comp->mSceneNode->getLocalTranslation());
    
    glm::vec4 debugShow;
    if(keyStates->isPressed(Input::Scancode::K_1)) {
        debugShow.x = 1.f;
    }
    if(keyStates->isPressed(Input::Scancode::K_2)) {
        debugShow.y = 1.f;
    }
    if(keyStates->isPressed(Input::Scancode::K_3)) {
        debugShow.z = 1.f;
    }
    if(keyStates->isPressed(Input::Scancode::K_4)) {
        debugShow.w = 1.f;
    }
    if(keyStates->isPressed(Input::Scancode::K_5)) {
        mDebugWireframe = true;
    }
    if(keyStates->isPressed(Input::Scancode::K_6)) {
        mDebugWireframe = false;
    }
    
    if(keyStates->isPressed(Input::Scancode::K_Q)) {
        mRenderer->setSunDirection(glm::vec3(mCamRollNode->calcWorldTransform() * glm::vec4(0.f, 0.f, -1.f, 0.f)));
    }
    
    mRenderer->setCameraViewMatrix(glm::inverse(mCamRollNode->calcWorldTransform()));
    
    // Although technically incorrect, it is visually better to do mouse-picking after the camera is updated
    
    // Make one of those selection rays
    Vec2 ndcMouse = mMouseLoc * 2.f - 1.f;
    ndcMouse.y = -ndcMouse.y;
    glm::vec4 rayStart = glm::vec4(ndcMouse.x, ndcMouse.y, 0.f, 1.f);
    glm::vec4 rayEnd = glm::vec4(ndcMouse.x, ndcMouse.y, 1.f, 1.f);
    glm::mat4 invCameraMatrix = glm::inverse(mRenderer->getCameraProjectionMatrix() * mRenderer->getCameraViewMatrix());
    rayStart = invCameraMatrix * rayStart;
    rayEnd = invCameraMatrix * rayEnd;
    rayStart /= rayStart.w;
    rayEnd /= rayEnd.w;
    
    Vec3 absStart = Vec3(rayStart);
    Vec3 absEnd = Vec3(rayEnd);
    
     // Perform ray test to determine what the legs are "standing on"
    btCollisionWorld::AllHitsRayResultCallback rayCallback(absStart, absEnd);
    mDynamicsWorld->rayTest(absStart, absEnd, rayCallback);

    //
    Vec3 hit;
    bool hitAnything = false;
    const btRigidBody* groundBody = nullptr;
    if(rayCallback.hasHit()) {
        // We cannot rely on the order of rayCallback.m_collisionObjects, so we have to compare the distances manually
        btScalar closestHitFraction(1337); // All fractions are <= 1 so this is effectively infinite
        for(int i = rayCallback.m_collisionObjects.size() - 1; i >= 0; -- i) {

            // If this result is closer than the closest valid result
            if(rayCallback.m_hitFractions.at(i) <= closestHitFraction) {
                // Get the object colliding with
                const btCollisionObject* other = rayCallback.m_collisionObjects.at(i);

                closestHitFraction = rayCallback.m_hitFractions.at(i);
                hit = rayCallback.m_hitPointWorld.at(i);
                groundBody = static_cast<const btRigidBody*>(other);
                
                hitAnything = true;
            }
        }
    }
    
    if(hitAnything) {
        mDebugCube->setLocalTranslation(hit);
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

