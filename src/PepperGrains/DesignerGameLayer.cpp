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
#include "Quate.hpp"
#include "MathUtil.hpp"
#include "InstancedModel.hpp"
#include "PepperGrains.hpp"
#include "SoundSource.hpp"
#include "SineWaveform.hpp"

namespace pgg {


DesignerGameLayer::Manipulator::Manipulator()
: handleDragged(-1)
, handleHovered(-1) {
}
DesignerGameLayer::Manipulator::~Manipulator() {
    
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

DesignerGameLayer::DesignerGameLayer(uint32_t width, uint32_t height)
: mScreenWidth(width)
, mScreenHeight(height) {
}

DesignerGameLayer::~DesignerGameLayer() {
}

void DesignerGameLayer::newSquarePlate(Vec3 location) {
    Plate* plate = new Plate();
    
    ResourceManager* resman = ResourceManager::getSingleton();
    
    plate->mSceneNode = mRootNode->newChild();
    plate->mSceneNode->grab();
    plate->mSceneNode->grabModel(resman->findModel("SquarePlate.model"));
    
    plate->mCollisionShape = new btBoxShape(Vec3(0.5f, 1.f / 60.f, 0.5f));
    plate->mMotionState = new btDefaultMotionState();
    plate->mCollisionObject = new btCollisionObject();
    plate->mCollisionObject->setCollisionShape(plate->mCollisionShape);
    plate->mCollisionObject->setUserPointer(plate);
    plate->mCollisionObject->getWorldTransform().setOrigin(Vec3(0.f, 0.f, 0.f));
    
    mCollisionWorld->addCollisionObject(plate->mCollisionObject);
    plate->mCollisionWorld = mCollisionWorld;
    
    // Set up edges
    {
        plate->mEdges.push_back(new StraightEdge(plate, Vec3(-0.5f, 0.f, -0.5f), Vec3(0.5f, 0.f, -0.5f)));
        plate->mEdges.push_back(new StraightEdge(plate, Vec3(0.5f, 0.f, -0.5f), Vec3(0.5f, 0.f, 0.5f)));
        plate->mEdges.push_back(new StraightEdge(plate, Vec3(0.5f, 0.f, 0.5f), Vec3(-0.5f, 0.f, 0.5f)));
        plate->mEdges.push_back(new StraightEdge(plate, Vec3(-0.5f, 0.f, 0.5f), Vec3(-0.5f, 0.f, -0.5f)));
    }
    
    // Set up sockets
    {
        for(uint8_t x = 1; x < 6; ++ x) {
            for(uint8_t z = 1; z < 6; ++ z) {
                
                float xf = (float) x;
                float zf = (float) z;
                
                plate->mSockets.push_back(new FlatSocket(plate, Vec3(xf * (1.f / 6.f) - 0.5f, 0.f, zf * (1.f / 6.f) - 0.5f), Vec3(0.f, 1.f, 0.f)));
                plate->mSockets.push_back(new FlatSocket(plate, Vec3(xf * (1.f / 6.f) - 0.5f, 0.f, zf * (1.f / 6.f) - 0.5f), Vec3(0.f, -1.f, 0.f)));
            }
        }
    }
    
    mPlates.push_back(plate);
    
    plate->setLocation(location, 1.f / 12.f);
}
void DesignerGameLayer::newMotor(Vec3 location) {
    Plate* plate = new Plate();
    
    ResourceManager* resman = ResourceManager::getSingleton();
    
    plate->mSceneNode = mRootNode->newChild();
    plate->mSceneNode->grab();
    plate->mSceneNode->grabModel(resman->findModel("Motor.model"));
    
    plate->mCollisionShape = new btCylinderShapeZ(Vec3(1.f / 4.f, 1.f / 4.f, 1.f / 3.f));
    plate->mMotionState = new btDefaultMotionState();
    plate->mCollisionObject = new btCollisionObject();
    plate->mCollisionObject->setCollisionShape(plate->mCollisionShape);
    plate->mCollisionObject->setUserPointer(plate);
    plate->mCollisionObject->getWorldTransform().setOrigin(Vec3(0.f, 0.f, 0.f));
    
    mCollisionWorld->addCollisionObject(plate->mCollisionObject);
    plate->mCollisionWorld = mCollisionWorld;
    
    // Set up sockets
    {
        float extent = 1.f / 3.f;
        
        plate->mSockets.push_back(new FlatSocket(plate, Vec3(extent, 0.f, 0.f), Vec3(1.f, 0.f, 0.f), true));
        plate->mSockets.push_back(new FlatSocket(plate, Vec3(-extent, 0.f, 0.f), Vec3(-1.f, 0.f, 0.f), true));
        plate->mSockets.push_back(new FlatSocket(plate, Vec3(0.f, extent, 0.f), Vec3(0.f, 1.f, 0.f), true));
        plate->mSockets.push_back(new FlatSocket(plate, Vec3(0.f, -extent, 0.f), Vec3(0.f, -1.f, 0.f), true));
        plate->mSockets.push_back(new FlatSocket(plate, Vec3(0.f, 0.f, extent), Vec3(0.f, 0.f, 1.f), true));
        plate->mSockets.push_back(new FlatSocket(plate, Vec3(0.f, 0.f, -extent), Vec3(0.f, 0.f, -1.f), true));
    }
    
    plate->mIsMotor = true;
    
    ModelResource* strutModel = resman->findModel("MotorStrut.model");
    plate->mMotorData.struts[0] = plate->mSceneNode->newChild()->grabModel(strutModel)->setVisible(false);
    plate->mMotorData.struts[1] = plate->mSceneNode->newChild()->grabModel(strutModel)->rotateRoll(glm::radians(180.f))->setVisible(false);
    plate->mMotorData.struts[2] = plate->mSceneNode->newChild()->grabModel(strutModel)->rotateRoll(glm::radians(90.f))->setVisible(false);
    plate->mMotorData.struts[3] = plate->mSceneNode->newChild()->grabModel(strutModel)->rotateRoll(glm::radians(-90.f))->setVisible(false);
    
    mPlates.push_back(plate);
    
    plate->setLocation(location, 1.f / 12.f);
}

void DesignerGameLayer::newCore(Vec3 location) {
    Plate* plate = new Plate();
    
    ResourceManager* resman = ResourceManager::getSingleton();
    
    plate->mSceneNode = mRootNode->newChild();
    plate->mSceneNode->grab();
    //plate->mSceneNode->grabModel(resman->findModel("Core.model"));
    plate->mSceneNode->grabModel(resman->findModel("SSIPGTest.model"));
    
    plate->mCollisionShape = new btBoxShape(Vec3(1.f, 1.f, 1.f));
    plate->mMotionState = new btDefaultMotionState();
    plate->mCollisionObject = new btCollisionObject();
    plate->mCollisionObject->setCollisionShape(plate->mCollisionShape);
    plate->mCollisionObject->setUserPointer(plate);
    plate->mCollisionObject->getWorldTransform().setOrigin(Vec3(0.f, 0.f, 0.f));
    
    mCollisionWorld->addCollisionObject(plate->mCollisionObject);
    plate->mCollisionWorld = mCollisionWorld;
    
    // Set up edges
    {
        plate->mEdges.push_back(new StraightEdge(plate, Vec3(-1.f, 1.f, -1.f), Vec3(1.f, 1.f, -1.f)));
        plate->mEdges.push_back(new StraightEdge(plate, Vec3(1.f, 1.f, -1.f), Vec3(1.f, 1.f, 1.f)));
        plate->mEdges.push_back(new StraightEdge(plate, Vec3(1.f, 1.f, 1.f), Vec3(-1.f, 1.f, 1.f)));
        plate->mEdges.push_back(new StraightEdge(plate, Vec3(-1.f, 1.f, 1.f), Vec3(-1.f, 1.f, -1.f)));
        
        plate->mEdges.push_back(new StraightEdge(plate, Vec3(-1.f, -1.f, -1.f), Vec3(1.f, -1.f, -1.f)));
        plate->mEdges.push_back(new StraightEdge(plate, Vec3(1.f, -1.f, -1.f), Vec3(1.f, -1.f, 1.f)));
        plate->mEdges.push_back(new StraightEdge(plate, Vec3(1.f, -1.f, 1.f), Vec3(-1.f, -1.f, 1.f)));
        plate->mEdges.push_back(new StraightEdge(plate, Vec3(-1.f, -1.f, 1.f), Vec3(-1.f, -1.f, -1.f)));
        
        plate->mEdges.push_back(new StraightEdge(plate, Vec3(-1.f, 1.f, -1.f), Vec3(-1.f, -1.f, -1.f)));
        plate->mEdges.push_back(new StraightEdge(plate, Vec3(1.f, 1.f, -1.f), Vec3(1.f, -1.f, -1.f)));
        plate->mEdges.push_back(new StraightEdge(plate, Vec3(1.f, 1.f, 1.f), Vec3(1.f, -1.f, 1.f)));
        plate->mEdges.push_back(new StraightEdge(plate, Vec3(-1.f, 1.f, 1.f), Vec3(-1.f, -1.f, 1.f)));
    }
    
    // Set up sockets
    {
        /*
        for(uint8_t x = 1; x < 6; ++ x) {
            for(uint8_t z = 1; z < 6; ++ z) {
                
                float xf = (float) x;
                float zf = (float) z;
                
                plate->mSockets.push_back(new FlatSocket(plate, Vec3(xf * (1.f / 6.f) - 0.5f, 0.f, zf * (1.f / 6.f) - 0.5f), Vec3(0.f, 1.f, 0.f)));
                plate->mSockets.push_back(new FlatSocket(plate, Vec3(xf * (1.f / 6.f) - 0.5f, 0.f, zf * (1.f / 6.f) - 0.5f), Vec3(0.f, -1.f, 0.f)));
            }
        }
        */
    }
    
    mPlates.push_back(plate);
    
    plate->setLocation(location, 1.f / 12.f);
}

void DesignerGameLayer::newConnector(Vec3 location) {
    ResourceManager* resman = ResourceManager::getSingleton();
    
    Plate* plates[2];
    
    for(uint8_t i = 0; i < 2; ++ i) {
        Plate* plate = new Plate();
        
        plate->mSceneNode = mRootNode->newChild();
        plate->mSceneNode->grab();
        plate->mSceneNode->grabModel(resman->findModel("ConnectorBase.model"));
        
        plate->mCollisionShape = new btSphereShape(1.f / 12.f);
        plate->mMotionState = new btDefaultMotionState();
        plate->mCollisionObject = new btCollisionObject();
        plate->mCollisionObject->setCollisionShape(plate->mCollisionShape);
        plate->mCollisionObject->setUserPointer(plate);
        plate->mCollisionObject->getWorldTransform().setOrigin(Vec3(0.f, 1.f / 12.f, 0.f));
        
        mCollisionWorld->addCollisionObject(plate->mCollisionObject);
        plate->mCollisionWorld = mCollisionWorld;
        
        plate->mSockets.push_back(new FlatSocket(plate, Vec3(0.f), Vec3(0.f, -1.f, 0.f), true));
        
        mPlates.push_back(plate);
        
        if(i == 0) {
            plate->setLocation(location, 1.f / 12.f);
        } else {
            plate->setLocation(location + Vec3(0.f, 1.f, 0.f), 1.f / 12.f);
        }
        
        if(i == 1) {
            plate->setIntermediateRoll(glm::radians(180.f));
            plate->finalizeRotation();
        }
        
        plate->mIsConnector = true;
        plate->mConnectorData.pipe = mRootNode->newChild();
        plate->mConnectorData.pipe->grab();
        
        if(i == 0) {
            plate->mConnectorData.isChief = true;
            plate->mConnectorData.pipe->grabModel(resman->findModel("ConnectorPipe.model"));
        } else {
            plate->mConnectorData.isChief = false;
        }
        
        plates[i] = plate;
    }
    
    plates[0]->mMultiplatePartners.push_back(plates[1]);
    plates[1]->mMultiplatePartners.push_back(plates[0]);
}

void DesignerGameLayer::newWheel(Vec3 location) {
    Plate* plate = new Plate();
    
    ResourceManager* resman = ResourceManager::getSingleton();
    
    plate->mSceneNode = mRootNode->newChild();
    plate->mSceneNode->grab();
    plate->mSceneNode->grabModel(resman->findModel("Wheel.model"));
    
    plate->mCollisionShape = new btCylinderShapeZ(Vec3(5.f / 12.f, 5.f / 12.f, 1.f / 6.f));
    plate->mMotionState = new btDefaultMotionState();
    plate->mCollisionObject = new btCollisionObject();
    plate->mCollisionObject->setCollisionShape(plate->mCollisionShape);
    plate->mCollisionObject->setUserPointer(plate);
    plate->mCollisionObject->getWorldTransform().setOrigin(Vec3(0.f, 0.f, 0.f));
    
    mCollisionWorld->addCollisionObject(plate->mCollisionObject);
    plate->mCollisionWorld = mCollisionWorld;
    
    mPlates.push_back(plate);
    
    plate->mSockets.push_back(new FlatSocket(plate, Vec3(0.f, 0.f, -1.f / 6.f), Vec3(0.f, 0.f, -1.f), true));
    
    plate->setLocation(location, 1.f / 12.f);
}
void DesignerGameLayer::deletePlate(Plate* plate) {
    mPlates.erase(std::remove(mPlates.begin(), mPlates.end(), plate), mPlates.end());
    
    for(std::vector<Edge*>::iterator iter = plate->mEdges.begin(); iter != plate->mEdges.end(); ++ iter) {
        Edge* edge = *iter;
        for(std::vector<Edge*>::iterator iter2 = edge->mLinks.begin(); iter2 != edge->mLinks.end(); ++ iter2) {
            Edge* other = *iter2;
            other->mLinks.erase(std::remove(other->mLinks.begin(), other->mLinks.end(), edge), other->mLinks.end());
        }
        
        // Should be fine to do this since the binding areas of the same plate are not supposed to be able to connect
        delete edge;
    }
    for(std::vector<Socket*>::iterator iter = plate->mSockets.begin(); iter != plate->mSockets.end(); ++ iter) {
        Socket* socket = *iter;
        for(std::vector<Socket*>::iterator iter2 = socket->mLinks.begin(); iter2 != socket->mLinks.end(); ++ iter2) {
            Socket* other = *iter2;
            other->mLinks.erase(std::remove(other->mLinks.begin(), other->mLinks.end(), socket), other->mLinks.end());
        }
        delete socket;
    }
    
    if(plate->mIsConnector) {
        plate->mConnectorData.pipe->drop();
    }
    
    plate->mSceneNode->drop();
    
    delete plate->mCollisionObject;
    delete plate->mMotionState;
    delete plate->mCollisionShape;
    
    delete plate;
}

// Lifecycle
void DesignerGameLayer::onBegin() {
    SDL_SetRelativeMouseMode(SDL_FALSE);
    
    mCollisionWorld = mCollisionPackage[0].mCollisionWorld;
    mManipulatorCollisionWorld = mCollisionPackage[1].mCollisionWorld;
    
    mSndEndpoint = PepperGrains::getSingleton()->mSndEndpoint;
    
    mPwi = mSndEndpoint->playWaveform(new SineWaveform(440.f), 0);
    
    
    ResourceManager* resman = ResourceManager::getSingleton();
    
    mRootNode = new SceneNode();
    mRootNode->grab();
    
    
    mRenderer = new DeferredRenderer(mScreenWidth, mScreenHeight);
    mRenderer->grab();
    
    mRenderer->setSkyColor(glm::vec3(0.f, 1.5f, 2.f));
    mRenderer->setSSAOEnabled(false);
    mRenderer->setShadowsEnabled(false);
    
    //mRootNode->newChild()->grabModel(resman->findModel("SSIPGTest.model"));
    
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
    
    mRenderer->setAmbientLight(glm::vec3(0.2, 0.2f, 0.2f));
    //mManipulator = resman->findModel("ManipulatorArrow.model");
    
    mDebugCube = mRootNode->newChild();
    mDebugCube->grabModel(resman->findModel("RoseCube.model"));
    mDebugCube->setLocalScale(Vec3(0.2, 0.2, 0.2));
    mDebugCube->grab();
    mDebugCube->setLocalTranslation(Vec3(999, 999, 999));
    
    mDebugShowX = false;
    mDebugShowY = false;
    mDebugShowZ = false;
    mDebugShowW = false;
    
    loadSlimeShader();
    loadManipulator();
    
    mInfCheck = new InfiniteCheckerboardModel();
    mInfCheck->grab();
    //mRootNode->newChild()->grabModel(mInfCheck);

    fps = 0.f;
    fpsWeight = 0.85f;
    
    mDebugWireframe = false;

    oneSecondTimer = 0.f;
    
    mPlateFreeDragged = nullptr;
    mPlateSelected = nullptr;
    
    mGridSize = 6;
    
    cyclicSawtooth = 0.f;
    cyclicSinusodal = 0.f;
    
    newCore(Vec3(0.f, 0.f, 0.f));
    newConnector(Vec3(1.f, 2.f, 1.f));
    newConnector(Vec3(1.f, 2.f, 1.f));
    newConnector(Vec3(1.f, 2.f, 1.f));
    newConnector(Vec3(1.f, 2.f, 1.f));
    newConnector(Vec3(1.f, 2.f, 1.f));
    newConnector(Vec3(1.f, 2.f, 1.f));
    newWheel(Vec3(1.f, 1.f, 1.f));
    newWheel(Vec3(1.f, 1.f, 1.f));
    newWheel(Vec3(1.f, 1.f, 1.f));
    newWheel(Vec3(1.f, 1.f, 1.f));
    newWheel(Vec3(1.f, 1.f, 1.f));
    newWheel(Vec3(1.f, 1.f, 1.f));
    newWheel(Vec3(1.f, 1.f, 1.f));
    newWheel(Vec3(1.f, 1.f, 1.f));
    newMotor(Vec3(1.f, 1.f, 1.f));
    newMotor(Vec3(1.f, 1.f, 1.f));
    newMotor(Vec3(1.f, 1.f, 1.f));
    newMotor(Vec3(1.f, 1.f, 1.f));
    newMotor(Vec3(1.f, 1.f, 1.f));
    newMotor(Vec3(1.f, 1.f, 1.f));
    newMotor(Vec3(1.f, 1.f, 1.f));
    newMotor(Vec3(1.f, 1.f, 1.f));
    newMotor(Vec3(1.f, 1.f, 1.f));
    newSquarePlate(Vec3(1.f, 1.f, 1.f));
    newSquarePlate(Vec3(1.f, 1.f, 1.f));
    newSquarePlate(Vec3(1.f, 1.f, 1.f));
    newSquarePlate(Vec3(1.f, 1.f, 1.f));
    newSquarePlate(Vec3(1.f, 1.f, 1.f));
    newSquarePlate(Vec3(1.f, 1.f, 1.f));
    newSquarePlate(Vec3(1.f, 1.f, 1.f));
    newSquarePlate(Vec3(1.f, 1.f, 1.f));
    newSquarePlate(Vec3(1.f, 1.f, 1.f));
    newSquarePlate(Vec3(1.f, 1.f, 1.f));
    newSquarePlate(Vec3(1.f, 1.f, 1.f));
}

void DesignerGameLayer::updateManipulatorTransform() {
    if(!mPlateSelected || mPlateFreeDragged) {
        return;
    }
    
    glm::mat4 cameraMatrix = mRenderer->getCameraProjectionMatrix() * mRenderer->getCameraViewMatrix();
    
    glm::vec4 asdf = cameraMatrix * glm::vec4(glm::vec3(mPlateSelected->mRenderLocation), 1.f);
    asdf /= asdf.w;

    float z = asdf.z * 2.f - 1.f;
    const float& near = mRenderer->getCameraNearDepth();
    const float& far = mRenderer->getCameraFarDepth();
    float linDepth = (2.f * near * far) / (far + near - z * (far - near));

    float clipRad = Math::cotangent(mRenderer->getCameraFOV() / 2.f) / linDepth;
    
    mManipulator.scale = 0.5f / clipRad;
    
    if(mManipulator.scale < 0.5f) {
        mManipulator.scale = 0.5f;
    }
    mManipulator.location = mPlateSelected->mRenderLocation;
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
    
    mSlimeShader.mSunAwareProg = resman->findShaderProgram("Manipulator.shaderProgram");
    mSlimeShader.mSunAwareProg->grab();
    {
        const std::vector<ShaderProgramResource::Control>& uniformVec3s = mSlimeShader.mSunAwareProg->getUniformVec3s();
        for(std::vector<ShaderProgramResource::Control>::const_iterator iter = uniformVec3s.begin(); iter != uniformVec3s.end(); ++ iter) {
            const ShaderProgramResource::Control& entry = *iter;
            if(entry.name == "color") {
                mSlimeShader.mSunAwareColorHandle = entry.handle;
            } else if(entry.name == "sunDirection") {
                mSlimeShader.mSunAwareSunDirectionHandle = entry.handle;
            }
        }
    }
    
    mSlimeShader.mSunUnawareProg = resman->findShaderProgram("MinimalColor.shaderProgram");
    mSlimeShader.mSunUnawareProg->grab();
    {
        const std::vector<ShaderProgramResource::Control>& uniformVec3s = mSlimeShader.mSunAwareProg->getUniformVec3s();
        for(std::vector<ShaderProgramResource::Control>::const_iterator iter = uniformVec3s.begin(); iter != uniformVec3s.end(); ++ iter) {
            const ShaderProgramResource::Control& entry = *iter;
            if(entry.name == "color") {
                mSlimeShader.mSunUnawareColorHandle = entry.handle;
            }
        }
    }
    
    
    mSlimeShader.mVertexBall = resman->findGeometry("VertexBall.geometry");
    mSlimeShader.mVertexBall->grab();
    mSlimeShader.mStraightEdge = resman->findGeometry("StraightEdge.geometry");
    mSlimeShader.mStraightEdge->grab();
    mSlimeShader.mFlatSocket = resman->findGeometry("FlatSocket.geometry");
    mSlimeShader.mFlatSocket->grab();
    mSlimeShader.mOmniSocket = resman->findGeometry("OmniSocket.geometry");
    mSlimeShader.mOmniSocket->grab();
    
    glGenVertexArrays(1, &mSlimeShader.mVertexBallVAO);
    glBindVertexArray(mSlimeShader.mVertexBallVAO);
    mSlimeShader.mVertexBall->bindBuffers();
    if(mSlimeShader.mSunAwareProg->needsPosAttrib()) {
        mSlimeShader.mVertexBall->enablePositionAttrib(mSlimeShader.mSunAwareProg->getPosAttrib());
    }
    if(mSlimeShader.mSunAwareProg->needsNormalAttrib()) {
        mSlimeShader.mVertexBall->enableNormalAttrib(mSlimeShader.mSunAwareProg->getNormalAttrib());
    }
    glBindVertexArray(0);
    
    glGenVertexArrays(1, &mSlimeShader.mStraightEdgeVAO);
    glBindVertexArray(mSlimeShader.mStraightEdgeVAO);
    mSlimeShader.mStraightEdge->bindBuffers();
    if(mSlimeShader.mSunAwareProg->needsPosAttrib()) {
        mSlimeShader.mStraightEdge->enablePositionAttrib(mSlimeShader.mSunAwareProg->getPosAttrib());
    }
    if(mSlimeShader.mSunAwareProg->needsNormalAttrib()) {
        mSlimeShader.mStraightEdge->enableNormalAttrib(mSlimeShader.mSunAwareProg->getNormalAttrib());
    }
    glBindVertexArray(0);
    
    glGenVertexArrays(1, &mSlimeShader.mFlatSocketVAO);
    glBindVertexArray(mSlimeShader.mFlatSocketVAO);
    mSlimeShader.mFlatSocket->bindBuffers();
    if(mSlimeShader.mSunUnawareProg->needsPosAttrib()) {
        mSlimeShader.mFlatSocket->enablePositionAttrib(mSlimeShader.mSunUnawareProg->getPosAttrib());
    }
    glBindVertexArray(0);
    
    glGenVertexArrays(1, &mSlimeShader.mOmniSocketVAO);
    glBindVertexArray(mSlimeShader.mOmniSocketVAO);
    mSlimeShader.mOmniSocket->bindBuffers();
    if(mSlimeShader.mSunAwareProg->needsPosAttrib()) {
        mSlimeShader.mOmniSocket->enablePositionAttrib(mSlimeShader.mSunAwareProg->getPosAttrib());
    }
    if(mSlimeShader.mSunAwareProg->needsNormalAttrib()) {
        mSlimeShader.mOmniSocket->enableNormalAttrib(mSlimeShader.mSunAwareProg->getNormalAttrib());
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
    if(mSlimeShader.mSunAwareProg->needsPosAttrib()) {
        mManipulator.arrow->enablePositionAttrib(mSlimeShader.mSunAwareProg->getPosAttrib());
    }
    if(mSlimeShader.mSunAwareProg->needsNormalAttrib()) {
        mManipulator.arrow->enableNormalAttrib(mSlimeShader.mSunAwareProg->getNormalAttrib());
    }
    glBindVertexArray(0);
    
    glGenVertexArrays(1, &mManipulator.wheelVAO);
    glBindVertexArray(mManipulator.wheelVAO);
    mManipulator.wheel->bindBuffers();
    if(mSlimeShader.mSunAwareProg->needsPosAttrib()) {
        mManipulator.wheel->enablePositionAttrib(mSlimeShader.mSunAwareProg->getPosAttrib());
    }
    if(mSlimeShader.mSunAwareProg->needsNormalAttrib()) {
        mManipulator.wheel->enableNormalAttrib(mSlimeShader.mSunAwareProg->getNormalAttrib());
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
    mSlimeShader.mSunAwareProg->drop();
    
    mSlimeShader.mVertexBall->drop();
    mSlimeShader.mStraightEdge->drop();
    mSlimeShader.mFlatSocket->drop();
    mSlimeShader.mOmniSocket->drop();
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
    
    {
        double asdf = (mMouseLoc.y * 1.4) + 0.1;
        if(asdf < 0.1) asdf = 0.1;
        if(asdf > 1.5) asdf = 1.5;
        mPwi->setPhaseLinearTerm(asdf);
    }
    
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
    
    float arrowKeyPanSpeed = 1.0;
    if(inputStates->isPressed(Input::Scancode::K_ARROW_UP)) {
        mCamPitchNode->rotatePitch(tpf * arrowKeyPanSpeed);
    }
    if(inputStates->isPressed(Input::Scancode::K_ARROW_DOWN)) {
        mCamPitchNode->rotatePitch(-tpf * arrowKeyPanSpeed);
    }
    if(inputStates->isPressed(Input::Scancode::K_ARROW_LEFT)) {
        mCamYawNode->rotateYaw(tpf * arrowKeyPanSpeed);
    }
    if(inputStates->isPressed(Input::Scancode::K_ARROW_RIGHT)) {
        mCamYawNode->rotateYaw(-tpf * arrowKeyPanSpeed);
    }
    

    // mInfCheck->setFocus(comp->mSceneNode->getLocalTranslation());
    
    glm::vec4 debugShow;
    if(mDebugShowX) {
        debugShow.x = 1.f;
    }
    if(mDebugShowY) {
        debugShow.y = 1.f;
    }
    if(mDebugShowZ) {
        debugShow.z = 1.f;
    }
    if(mDebugShowW) {
        debugShow.w = 1.f;
    }
    {
        bool enabling = inputStates->isPressed(Input::Scancode::K_SHIFT_LEFT);
        if(inputStates->isPressed(Input::Scancode::K_1)) {
            mDebugShowX = enabling;
        }
        if(inputStates->isPressed(Input::Scancode::K_2)) {
            mDebugShowY = enabling;
        }
        if(inputStates->isPressed(Input::Scancode::K_3)) {
            mDebugShowZ = enabling;
        }
        if(inputStates->isPressed(Input::Scancode::K_4)) {
            mDebugShowW = enabling;
        }
        if(inputStates->isPressed(Input::Scancode::K_5)) {
            mDebugWireframe = enabling;
        }
        if(inputStates->isPressed(Input::Scancode::K_6)) {
            mRenderer->setSSAOEnabled(enabling);
        }
        if(inputStates->isPressed(Input::Scancode::K_7)) {
            mRenderer->setShadowsEnabled(enabling);
        }
    }
    
    mShowAllEdges = inputStates->isPressed(Input::Scancode::K_H);
    
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
        
        mManipulator.handleHovered = -1;
        if(rayCallback.hasHit()) {
            // Cannot rely on the order of rayCallback.m_collisionObjects, so we have to compare the distances manually
            btScalar closestHitFraction(1337); // All fractions are <= 1 so this is effectively infinite
            for(int i = rayCallback.m_collisionObjects.size() - 1; i >= 0; -- i) {
                if(rayCallback.m_hitFractions.at(i) <= closestHitFraction) {
                    const btCollisionObject* touched = rayCallback.m_collisionObjects.at(i);
                    
                    bool valid = false;
                    for(int8_t i = 0; i < 6; ++ i) {
                        if(mManipulator.collisionObjects[i] == touched) {
                            if(i <= 3 && mPlateSelected->mAllowManualLocation) {
                                mManipulator.handleHovered = i;
                                valid = true;
                            }
                            if(i > 3 && mPlateSelected->mAllowManualRotation) {
                                mManipulator.handleHovered = i;
                                valid = true;
                            }
                            break;
                        }
                    }
                    if(valid) {
                        closestHitFraction = rayCallback.m_hitFractions.at(i);
                        hitPoint = rayCallback.m_hitPointWorld.at(i);
                    }
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
    
    float mouseLookSensitivity = 0.002;
    if(inputStates->isPressed(Input::Scancode::M_RIGHT)) {
        mCamPitchNode->rotatePitch(((float) inputStates->getMouseDY()) * mouseLookSensitivity);
        mCamYawNode->rotateYaw(((float) inputStates->getMouseDX()) * mouseLookSensitivity);
    }
    
    if(inputStates->isPressed(Input::Scancode::M_LEFT)) {
        // First click...
        if(!mMouseLeftDownLastFrame) {
            // ...on a plate
            if(mPlateHovered) {
                selectPlate(mPlateHovered);
                mPlateFreeDragged = mPlateHovered;
                mPlateDragPoint = hitPoint - mPlateHovered->mLocation;
                
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
                    Vec3 worldA = mPlateSelected->mLocation;
                    Vec3 worldB = worldA;
                    
                    if(mManipulator.handleDragged == 0) {
                        worldB.x += 1.f;
                    } else if(mManipulator.handleDragged == 1) {
                        worldB.y += 1.f;
                    } else {
                        worldB.z += 1.f;
                    }
                    
                    float frac = Math::nearest3DLineIntersection(worldA, worldB, mouseRayStart, mouseRayEnd);
                    if(frac < -100.f) {
                        frac = -100.f;
                    } else if(frac > 100.f) {
                        frac = 100.f;
                    }
                    
                    mManipulator.initialAxisDragFrac = frac;
                }
                
                // Wheel
                else {
                    
                    Vec3 origin = mPlateSelected->mRenderLocation;
                    
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
                    Vec3 worldA = mPlateSelected->mLocation;
                    Vec3 worldB = worldA;
                    
                    if(mManipulator.handleDragged == 0) {
                        worldB.x += 1.f;
                    } else if(mManipulator.handleDragged == 1) {
                        worldB.y += 1.f;
                    } else {
                        worldB.z += 1.f;
                    }
                    
                    float frac = Math::nearest3DLineIntersection(worldA, worldB, mouseRayStart, mouseRayEnd);
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
                    
                    Vec3 origin = mPlateSelected->mRenderLocation;
                    
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
        if(mManipulator.handleDragged >= 3) {
            mPlateSelected->finalizeRotation();
        }
        mManipulator.handleDragged = -1;
    }
    
    for(std::vector<Plate*>::iterator iter = mPlates.begin(); iter != mPlates.end(); ++ iter) {
        Plate* targetLoc = *iter;
        targetLoc->tick(tpf);
    }
    for(std::vector<Plate*>::iterator iter = mPlates.begin(); iter != mPlates.end(); ++ iter) {
        Plate* targetLoc = *iter;
        targetLoc->rebuildLinks(mPlates);
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
    glClear(GL_DEPTH_BUFFER_BIT);
    glDepthMask(GL_FALSE);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_STENCIL_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_CONSTANT_COLOR, GL_ONE_MINUS_CONSTANT_COLOR);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    
    updateManipulatorTransform();

    
    if(mPlates.size() > 0 && mShowAllEdges) {
        glDepthMask(GL_TRUE);
        glEnable(GL_DEPTH_TEST);
        for(std::vector<Plate*>::iterator iter = mPlates.begin(); iter != mPlates.end(); ++ iter) {
            Plate* thisPlate = *iter;
            if(thisPlate == mPlateSelected) { continue; }
            thisPlate->renderEdges(mRenderer, mSlimeShader);
            thisPlate->renderSockets(mRenderer, mSlimeShader);
        }
        glDepthMask(GL_FALSE);
        glDisable(GL_DEPTH_TEST);
    }
    
    if(mPlateSelected) {
        glDepthMask(GL_TRUE);
        glEnable(GL_DEPTH_TEST);
        mPlateSelected->renderEdges(mRenderer, mSlimeShader);
        mPlateSelected->renderSockets(mRenderer, mSlimeShader);
        glDepthMask(GL_TRUE);
        glDisable(GL_DEPTH_TEST);
    }
    
    if(mPlateSelected && !mPlateFreeDragged) {
        float alphaDefault = 0.3f;
        float alphaHover = cyclicSinusodal * 0.5f + 0.3f;
        glBlendColor(alphaDefault, alphaDefault, alphaDefault, 1.0f);
        
        SceneNode* mUtilityNode = new SceneNode();
        mUtilityNode->grab();
        mUtilityNode->setLocalTranslation(mManipulator.location);
        mUtilityNode->setLocalScale(Vec3(mManipulator.scale));
        mUtilityNode->rotateYaw(glm::radians(90.f));

        glUseProgram(mSlimeShader.mSunAwareProg->getHandle());
        
        glUniform3fv(mSlimeShader.mSunAwareSunDirectionHandle, 1, glm::value_ptr(mRenderer->getSunDirection() * -1.f));
        
        for(int8_t i = 0; i < 3; ++ i) {
            if(i == 0) {
                glUniform3fv(mSlimeShader.mSunAwareColorHandle, 1, glm::value_ptr(glm::vec3(1.f, 0.f, 0.f)));
            }
            else if(i == 1) {
                glUniform3fv(mSlimeShader.mSunAwareColorHandle, 1, glm::value_ptr(glm::vec3(0.f, 1.f, 0.f)));
                mUtilityNode->rotateYaw(glm::radians(-90.f));
                mUtilityNode->rotatePitch(glm::radians(-90.f));
            }
            else if(i == 2) {
                glUniform3fv(mSlimeShader.mSunAwareColorHandle, 1, glm::value_ptr(glm::vec3(0.f, 0.f, 1.f)));
                mUtilityNode->rotatePitch(glm::radians(90.f));
            }
            
            mSlimeShader.mSunAwareProg->bindModelViewProjMatrices(mUtilityNode->calcWorldTransform(), mRenderer->getCameraViewMatrix(), mRenderer->getCameraProjectionMatrix());
            
            if(mPlateSelected->mAllowManualLocation) {
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
            }
            
            if(mPlateSelected->mAllowManualRotation) {
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
        }
        
        glBindVertexArray(0);
        glUseProgram(0);
        mUtilityNode->drop();
    }
    
}

bool DesignerGameLayer::onWindowSizeUpdate(const WindowResizeEvent& event) {
    return true;
}
}

