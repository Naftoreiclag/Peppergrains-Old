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
    SDL_SetRelativeMouseMode(SDL_TRUE);
    
    ResourceManager* resman = ResourceManager::getSingleton();
    
    mRenderer = new DeferredRenderer(mScreenWidth, mScreenHeight);
    mRenderer->grab();

    mEntityWorld = new nres::World();

    mBroadphase = new btDbvtBroadphase();
    mCollisionConfiguration = new btDefaultCollisionConfiguration();
    mDispatcher = new btCollisionDispatcher(mCollisionConfiguration);
    mSolver = new btSequentialImpulseConstraintSolver();
    mDynamicsWorld = new btDiscreteDynamicsWorld(mDispatcher, mBroadphase, mSolver, mCollisionConfiguration);
    mDynamicsWorld->setGravity(btVector3(0, -9.8, 0));
    mRigidBodyESys = new RigidBodyESys(mDynamicsWorld);
    mEntityWorld->attachSystem(mRigidBodyESys);
    
    btStaticPlaneShape* planeShape = new btStaticPlaneShape(btVector3(0, 1, 0), 0);
    mPlaneRigid = new btRigidBody(0, 0, planeShape);
    mDynamicsWorld->addRigidBody(mPlaneRigid);
    
    mRootNode = new SceneNode();
    mRootNode->grab();
    mSceneNodeESys = new SceneNodeESys(mRootNode);
    mEntityWorld->attachSystem(mSceneNodeESys);
    
    mPlayerEntity = mEntityWorld->newEntity();
    mPlayerEntity->add(new SceneNodeEComp());
    mPlayerEntity->addListener(new DebugFPControllerEListe());
    mPlayerEntity->publish();
    
    nres::Entity* cube;
    cube = mEntityWorld->newEntity();
    cube->add(new SceneNodeEComp(resman->findModel("RoseCube.model")));
    cube->add(new RigidBodyEComp(new btBoxShape(Vec3(1.f, 1.f, 1.f)), Vec3(-3.f, 3.f, -3.f)));
    cube->publish();
    
    cube = mEntityWorld->newEntity();
    cube->add(new SceneNodeEComp(resman->findModel("RoseCube.model")));
    cube->add(new RigidBodyEComp(new btBoxShape(Vec3(1.f, 1.f, 1.f)), Vec3(-4.5f, 8.f, -4.5f)));
    cube->publish();
    
    cube = mEntityWorld->newEntity();
    cube->add(new SceneNodeEComp(resman->findModel("RoseCube.model")));
    cube->add(new RigidBodyEComp(new btBoxShape(Vec3(1.f, 1.f, 1.f)), Vec3(-4.5f, 8.f, -4.5f)));
    cube->publish();
    
    mRootNode->newChild()->grabModel(new GrassModel());
    mRootNode->newChild()->move(glm::vec3(1.5f, 1.5f, 1.5f))->grabModel(new PointLightModel(glm::vec3(2.f, 0.f, 2.f), 2.f));
    mRootNode->newChild()->move(glm::vec3(-3.f, 3.f, -3.f))->grabModel(new TessModel());
    mRootNode->newChild()->move(glm::vec3(-3.f, 3.f, -3.f))->grabModel(resman->findModel("Door.model"));
    
    
    rainstormFont = resman->findFont("Rainstorm.font");
    rainstormFont->grab();

    fpsCounter = new TextModel(rainstormFont, "FPS: Calculating...");
    fpsCounter->grab();
    
    mCamLocNode = new SceneNode();
    mCamLocNode->move(Vec3(0.f, 1.5f, 0.f));
    
    mCamRollNode = new SceneNode();
    mCamPitchNode = new SceneNode();
    mCamYawNode = new SceneNode();
    mCamLocNode->addChild(mCamYawNode);
    mCamYawNode->addChild(mCamPitchNode);
    mCamPitchNode->addChild(mCamRollNode);
    
    SceneNodeEComp* plS = (SceneNodeEComp*) mPlayerEntity->getComponent(SceneNodeEComp::sComponentID);
    plS->mSceneNode->addChild(mCamLocNode);
    
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
    
    fpsCounter->drop();
    rainstormFont->drop();
    delete mSceneNodeESys;
    delete mRigidBodyESys;
    delete mDynamicsWorld;
    delete mSolver;
    delete mDispatcher;
    delete mCollisionConfiguration;
    delete mBroadphase;
}

void DesignerGameLayer::onTick(float tpf, const Uint8* keyStates) {
    
    glm::vec3 movement;
    if(keyStates[SDL_GetScancodeFromKey(SDLK_w)]) {
        movement.z -= 1.0;
    }
    if(keyStates[SDL_GetScancodeFromKey(SDLK_a)]) {
        movement.x -= 1.0;
    }
    if(keyStates[SDL_GetScancodeFromKey(SDLK_s)]) {
        movement.z += 1.0;
    }
    if(keyStates[SDL_GetScancodeFromKey(SDLK_d)]) {
        movement.x += 1.0;
    }
    if(movement != glm::vec3(0.f)) {
        glm::normalize(movement);
        if(keyStates[SDL_GetScancodeFromKey(SDLK_LSHIFT)]) {
            movement *= 10.f;
        }
        
        movement = glm::vec3(mCamRollNode->calcWorldTransform() * glm::vec4(movement, 0.f) * tpf);
        
        mPlayerEntity->broadcast(new InputMoveESignal(movement));
    }

    SceneNodeEComp* comp = (SceneNodeEComp*) mPlayerEntity->getComponent(SceneNodeEComp::sComponentID);
    mInfCheck->setFocus(comp->mSceneNode->getLocalTranslation());
    
    glm::vec4 debugShow;
    if(keyStates[SDL_GetScancodeFromKey(SDLK_1)]) {
        debugShow.x = 1.f;
    }
    if(keyStates[SDL_GetScancodeFromKey(SDLK_2)]) {
        debugShow.y = 1.f;
    }
    if(keyStates[SDL_GetScancodeFromKey(SDLK_3)]) {
        debugShow.z = 1.f;
    }
    if(keyStates[SDL_GetScancodeFromKey(SDLK_4)]) {
        debugShow.w = 1.f;
    }
    if(keyStates[SDL_GetScancodeFromKey(SDLK_5)]) {
        mDebugWireframe = true;
    }
    if(keyStates[SDL_GetScancodeFromKey(SDLK_6)]) {
        mDebugWireframe = false;
    }
    
    if(keyStates[SDL_GetScancodeFromKey(SDLK_q)]) {
        mRenderer->setSunDirection(glm::vec3(mCamRollNode->calcWorldTransform() * glm::vec4(0.f, 0.f, -1.f, 0.f)));
    }
    
    if(keyStates[SDL_GetScancodeFromKey(SDLK_r)]) {
        ResourceManager* resman = ResourceManager::getSingleton();
        
        nres::Entity* cube;
        cube = mEntityWorld->newEntity();
        cube->add(new SceneNodeEComp(resman->findModel("RoseCube.model")));
        cube->add(new RigidBodyEComp(new btBoxShape(Vec3(1.f, 1.f, 1.f)), Vec3(0.f, 3.f, 0.f) + mCamLocNode->calcWorldTranslation()));
        cube->publish();
    }
    mDynamicsWorld->stepSimulation(tpf, 5);
    mRigidBodyESys->onTick();
    mSceneNodeESys->onTick(tpf);
    
    mRenderer->setCameraViewMatrix(glm::inverse(mCamRollNode->calcWorldTransform()));
    
    mRenderer->renderFrame(mRootNode, debugShow, mDebugWireframe);
    
    if(tpf > 0) {
        float fpsNew = 1 / tpf;
        fps = (fps * fpsWeight) + (fpsNew * (1.f - fpsWeight));
    }

    oneSecondTimer += tpf;
    if(oneSecondTimer > 1.f) {
        oneSecondTimer -= 1.f;

        fpsCounter->drop();

        std::stringstream ss;
        ss << "FPS: ";
        ss << (uint32_t) fps;

        fpsCounter = new TextModel(rainstormFont, ss.str());
        fpsCounter->grab();
        
        std::cout << "Cam location: " << glm::to_string(mCamLocNode->calcWorldTransform()[3]) << std::endl;
    }
    
    glm::mat4 viewMatOverlay;
    glm::mat4 projMatOverlay = glm::ortho(0.f, (float) mScreenWidth, 0.f, (float) mScreenHeight);
    
    Model::RenderPass fpsRPC(Model::RenderPassType::SCREEN);
    fpsRPC.viewMat = viewMatOverlay;
    fpsRPC.projMat = projMatOverlay;
    fpsCounter->render(fpsRPC, glm::mat4());
}

bool DesignerGameLayer::onMouseMove(const SDL_MouseMotionEvent& event) {
    // float x = event.x;
    // float y = event.y;
    float dx = event.xrel;
    float dy = event.yrel;
    
    mCamYawNode->rotateYaw(-dx * 0.003f);
    mCamPitchNode->rotatePitch(-dy * 0.003f);
    
    return true;
}

bool DesignerGameLayer::onWindowSizeUpdate(const SDL_WindowEvent& event) {
    std::cout << event.data1 << ", " << event.data2 << std::endl;
    
    return true;
}
}

