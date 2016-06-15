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

float cotangent(const float& radians) {
    return std::cos(radians) / std::sin(radians);
}

glm::quat quaternionLookAt(Vec3 targetDirection, Vec3 initialDirection, Vec3 upDirection) {
    float dotProd = initialDirection.dot(targetDirection);
    // Already facing direction
    if(std::abs(1.f - dotProd) < 0.000001f) {
        return glm::quat();
    }
    // Facing 180 degrees in the wrong direction
    else if(std::abs(-1.f - dotProd) < 0.000001f) {
        return glm::angleAxis(3.1416f, glm::vec3(upDirection));
    }
    return glm::angleAxis(std::acos(dotProd), glm::vec3(initialDirection.cross(targetDirection).normalized()));
}

DesignerGameLayer::Edge::Edge(Type type, Plate* plate)
: mType(type)
, mPlate(plate) {
    
}

DesignerGameLayer::Edge::~Edge() {
    
}

DesignerGameLayer::StraightEdge::StraightEdge(Plate* plate, const Vec3& start, const Vec3& end)
: Edge(Edge::Type::STRAIGHT, plate)
, mStartLoc(start)
, mEndLoc(end) {
}

DesignerGameLayer::StraightEdge::~StraightEdge() {
    
}
void DesignerGameLayer::StraightEdge::onPlateChangeTransform(const Vec3& location, const glm::quat& orientation) {
    mWorldStartLoc = Vec3(glm::mat4_cast(orientation) * glm::vec4(glm::vec3(mStartLoc), 1.0));
    mWorldEndLoc = Vec3(glm::mat4_cast(orientation) * glm::vec4(glm::vec3(mEndLoc), 1.0));
    
    mWorldStartLoc += location;
    mWorldEndLoc += location;
}

bool linesIntersect(Vec3 A, Vec3 B, Vec3 C, Vec3 D) {
    
    Vec3 myDisplacement = B - A;
    float myMagnitude = myDisplacement.mag();
    Vec3 myDirection = myDisplacement / myMagnitude;
    
    Vec3 otherDirection = D - C;
    otherDirection.normalize();
    
    // Cosine of the angle between the directions of both edges
    float angleCos = myDirection.dot(otherDirection);
    
    // Check if lines could be parallel enough (both directions are either both nearly the same or are 180 degrees apart)
    if(std::abs(1.f - angleCos) < 0.0001f || std::abs(-1.f - angleCos) < 0.0001f) {
    }
    else {
        // Not at all parallellish
        return false;
    }
    
    // Check if the magnitude of the cross product between these two edges is close to zero
    {
        // This point a point on the other line which is not too close from the start point
        Vec3 otherPoint;
        
        // Make sure this other point is far away enough to get meaningful data from the cross product
        if((C - A).magSq() < 0.0001f) { // Using magSq to avoid division by zero
            otherPoint = D;
        } else {
            otherPoint = C;
        }
        
        float crossProdMagSq = ((otherPoint - A).cross(myDisplacement)).magSq();
        
        if(crossProdMagSq > 0.0001f) {
            return false;
        }
    }
    
    float fracS = (C - A).dot(myDirection) / myMagnitude;
    float fracE = (D - A).dot(myDirection) / myMagnitude;
    
    /* Valid cases:
     * 
     * Caught by the first check:
     *       A---------A
     *    B-----B
     *       A---------A
     *       B----B
     *       A---------A
     *          B---B
     *       A---------A
     *             B---B
     *       A---------A
     *               B-----B
     * Caught by the second check:
     *       A---------A
     *       B---------B
     * 
     * Caught by the third check:
     *       A---------A
     *    B---------------B
     */
     
    bool onEdgeS = abs(fracS) < 0.0001f || abs(1 - fracS) < 0.0001f;
    bool insideS = fracS > 0.f && fracS < 1.f && !onEdgeS;
    bool outsideS = !insideS && !onEdgeS;
    
    bool onEdgeE = abs(fracE) < 0.0001f || abs(1 - fracE) < 0.0001f;
    bool insideE = fracE > 0.f && fracE < 1.f && !onEdgeE;
    bool outsideE = !insideE && !onEdgeE;
    
    // If one of the points are within this edge, then the two connect
    if(insideE || insideS) { return true; }
    
    // If both points line up with this edge exactly, then the two connect
    if(onEdgeS && onEdgeE) { return true; }
    
    // If both points are outside, then two connect only if the points are on opposite sides
    if(outsideS && outsideE && ((fracS < 0.f && fracE > 1.f) || (fracE < 0.f && fracS > 1.f))) { return true; }
    
    return false;
    
}

bool DesignerGameLayer::StraightEdge::canBindTo(Edge* otherEdge) const {
    
    if(otherEdge->mType != Edge::Type::STRAIGHT) {
        return false;
    }
    
    StraightEdge* other = (StraightEdge*) otherEdge;
    
    return linesIntersect(mWorldStartLoc, mWorldEndLoc, other->mWorldStartLoc, other->mWorldEndLoc);
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
, mIntegralZ(0)
, mIntegralScaleX(1)
, mIntegralScaleY(1)
, mIntegralScaleZ(1)
, intermediatePitch(0.f)
, intermediateYaw(0.f)
, intermediateRoll(0.f)
, needRebuildUnionGraph(false) { }
DesignerGameLayer::Plate::~Plate() { }

void DesignerGameLayer::Plate::setIntermediatePitch(float radians) {
    intermediateYaw = 0;
    intermediateRoll = 0;
    float newRot = nearestRightAngle(radians);
    if(intermediatePitch != newRot) {
        intermediatePitch = newRot;
        mOrientation = glm::angleAxis(intermediatePitch, glm::vec3(1.f, 0.f, 0.f)) * mFinalizedOrienation;
        onTransformChanged();
    }
}
void DesignerGameLayer::Plate::setIntermediateYaw(float radians) {
    intermediatePitch = 0;
    intermediateRoll = 0;
    float newRot = nearestRightAngle(radians);
    if(intermediateYaw != newRot) {
        intermediateYaw = newRot;
        mOrientation = glm::angleAxis(intermediateYaw, glm::vec3(0.f, 1.f, 0.f)) * mFinalizedOrienation;
        onTransformChanged();
    }
}
void DesignerGameLayer::Plate::setIntermediateRoll(float radians) {
    intermediatePitch = 0;
    intermediateYaw = 0;
    float newRot = nearestRightAngle(radians);
    if(intermediateRoll != newRot) {
        intermediateRoll = newRot;
        mOrientation = glm::angleAxis(intermediateRoll, glm::vec3(0.f, 0.f, 1.f)) * mFinalizedOrienation;
        onTransformChanged();
    }
}
void DesignerGameLayer::Plate::finalizeRotation() {
    mFinalizedOrienation = mOrientation;
    
    intermediatePitch = 0.f;
    intermediateYaw = 0.f;
    intermediateRoll = 0.f;
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

void DesignerGameLayer::Plate::onTransformChanged() {
    needRebuildUnionGraph = true;
    
    for(std::vector<Edge*>::iterator iter2 = mEdges.begin(); iter2 != mEdges.end(); ++ iter2) {
        Edge* myEdge = *iter2;
        
        myEdge->onPlateChangeTransform(mLocation, mOrientation);
    }
}

void DesignerGameLayer::Plate::rebuildUnionGraph(std::vector<Plate*>& plates) {
    if(!needRebuildUnionGraph) {
        return;
    }

    
    for(std::vector<Edge*>::iterator myEdgesIterator = mEdges.begin(); myEdgesIterator != mEdges.end(); ++ myEdgesIterator) {
        Edge* myEdge = *myEdgesIterator;

        // Disconnect from all edges
        for(std::vector<Edge*>::iterator myEdgesUnionsIterator = myEdge->mUnions.begin(); myEdgesUnionsIterator != myEdge->mUnions.end(); ++ myEdgesUnionsIterator) {
            Edge* otherEdge = *myEdgesUnionsIterator;
            
            otherEdge->mUnions.erase(std::remove(otherEdge->mUnions.begin(), otherEdge->mUnions.end(), myEdge), otherEdge->mUnions.end());
        }
        myEdge->mUnions.clear();
        
        for(std::vector<Plate*>::iterator otherPlateIterator = plates.begin(); otherPlateIterator != plates.end(); ++ otherPlateIterator) {
            Plate* other = *otherPlateIterator;
            // Skip self
            if(other == this) {
                continue;
            }

            // Connect (or reconnect) to edges that are valid
            for(std::vector<Edge*>::iterator otherPlateEdgesIterator = other->mEdges.begin(); otherPlateEdgesIterator != other->mEdges.end(); ++ otherPlateEdgesIterator) {
                Edge* otherEdge = *otherPlateEdgesIterator;
                
                // Skip self
                if(myEdge == otherEdge) {
                    continue;
                }
                
                if(myEdge->canBindTo(otherEdge) && otherEdge->canBindTo(myEdge)) {
                    myEdge->mUnions.push_back(otherEdge);
                    otherEdge->mUnions.push_back(myEdge);
                }
            }
        }
    }
    
    needRebuildUnionGraph = false;
}

void DesignerGameLayer::Plate::setLocation(Vec3 location, float snapSize) {
    uint32_t newIntegralX = (uint32_t) std::floor(toNearestMultiple(location.x, snapSize) * 12.f + 0.5);
    uint32_t newIntegralY = (uint32_t) std::floor(toNearestMultiple(location.y, snapSize) * 12.f + 0.5);
    uint32_t newIntegralZ = (uint32_t) std::floor(toNearestMultiple(location.z, snapSize) * 12.f + 0.5);
    
    if(mIntegralX != newIntegralX || mIntegralY != newIntegralY || mIntegralZ != newIntegralZ) {
        mIntegralX = newIntegralX;
        mIntegralY = newIntegralY;
        mIntegralZ = newIntegralZ;
        mLocation = Vec3((float) mIntegralX, (float) mIntegralY, (float) mIntegralZ);
        mLocation /= 12.f;
        onTransformChanged();
    }
}

void DesignerGameLayer::Plate::tick(float tpf) {
    
    if((mLocation - mRenderLocation).mag() < tpf) {
        mRenderLocation = mLocation;
    } else {
        mRenderLocation += (mLocation - mRenderLocation) * tpf * 15.f;
    }
    
    mRenderOrientation = glm::slerp(mRenderOrientation, mOrientation, tpf * 15.f);
    
    mSceneNode->setLocalTranslation(mRenderLocation);
    mSceneNode->setLocalOrientation(mRenderOrientation);
    
    collisionObject->getWorldTransform().setOrigin(mRenderLocation);
    collisionObject->getWorldTransform().setRotation(Quate(mRenderOrientation));
    collisionWorld->removeCollisionObject(collisionObject);
    collisionWorld->addCollisionObject(collisionObject);
}

DesignerGameLayer::DesignerGameLayer(uint32_t width, uint32_t height)
: mScreenWidth(width)
, mScreenHeight(height) {
}

DesignerGameLayer::~DesignerGameLayer() {
}

void DesignerGameLayer::newPlate(Vec3 location) {
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
        plate->mEdges.push_back(new StraightEdge(plate, Vec3(-0.5f, 0.f, -0.5f), Vec3(0.5f, 0.f, -0.5f)));
        plate->mEdges.push_back(new StraightEdge(plate, Vec3(0.5f, 0.f, -0.5f), Vec3(0.5f, 0.f, 0.5f)));
        plate->mEdges.push_back(new StraightEdge(plate, Vec3(0.5f, 0.f, 0.5f), Vec3(-0.5f, 0.f, 0.5f)));
        plate->mEdges.push_back(new StraightEdge(plate, Vec3(-0.5f, 0.f, 0.5f), Vec3(-0.5f, 0.f, -0.5f)));
    }
    
    mPlates.push_back(plate);
    
    plate->setLocation(location, 1.f / 12.f);
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
    
    newPlate(Vec3(1.f, 1.f, 1.f));
    newPlate(Vec3(1.f, 2.f, 1.f));
    newPlate(Vec3(1.f, 3.f, 1.f));
    newPlate(Vec3(1.f, 4.f, 1.f));
    newPlate(Vec3(1.f, 5.f, 1.f));
    newPlate(Vec3(1.f, 6.f, 1.f));
    newPlate(Vec3(1.f, 7.f, 1.f));
    newPlate(Vec3(1.f, 8.f, 1.f));
    newPlate(Vec3(1.f, 9.f, 1.f));
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

    float clipRad = cotangent(mRenderer->getCameraFOV() / 2.f) / linDepth;
    
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
        targetLoc->rebuildUnionGraph(mPlates);
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
    glDepthMask(GL_FALSE);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_STENCIL_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_CONSTANT_COLOR, GL_ONE_MINUS_CONSTANT_COLOR);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    // glClear(GL_DEPTH_BUFFER_BIT);
    
    updateManipulatorTransform();

    float alphaDefault = 0.3f;
    float alphaHover = cyclicSinusodal * 0.5f + 0.3f;
    glBlendColor(alphaDefault, alphaDefault, alphaDefault, 1.0f);
    
    if(mPlates.size() > 0) {
        
        glDepthMask(GL_FALSE);
        glDisable(GL_DEPTH_TEST);
        
        for(std::vector<Plate*>::iterator iter = mPlates.begin(); iter != mPlates.end(); ++ iter) {
            Plate* thisPlate = *iter;
            
            thisPlate->renderEdges(mRenderer, mSlimeShader);
            
        }
        glClear(GL_DEPTH_BUFFER_BIT);
        glDepthMask(GL_FALSE);
        glDisable(GL_DEPTH_TEST);
    }
    
    /*
    if(mPlateSelected) {
        glDepthMask(GL_TRUE);
        glEnable(GL_DEPTH_TEST);
        glClear(GL_DEPTH_BUFFER_BIT);
        mPlateSelected->renderEdges(mRenderer, mSlimeShader);
        glClear(GL_DEPTH_BUFFER_BIT);
        glDepthMask(GL_TRUE);
        glDisable(GL_DEPTH_TEST);
    }
    */
    
    if(mPlateSelected && !mPlateFreeDragged) {
        SceneNode* mUtilityNode = new SceneNode();
        mUtilityNode->grab();
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
            
            mSlimeShader.mShaderProg->bindModelViewProjMatrices(mUtilityNode->calcWorldTransform(), mRenderer->getCameraViewMatrix(), mRenderer->getCameraProjectionMatrix());
            
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
        mUtilityNode->drop();
        // glClear(GL_DEPTH_BUFFER_BIT);
    }
    
}

void DesignerGameLayer::Plate::renderEdges(const DeferredRenderer* mRenderer, const SlimeShader& mSlimeShader) const {
    for(std::vector<Edge*>::const_iterator iter = mEdges.cbegin(); iter != mEdges.cend(); ++ iter) {
        const Edge* edge = *iter;
        edge->renderVertices(mRenderer, mSlimeShader);
    }
    for(std::vector<Edge*>::const_iterator iter = mEdges.cbegin(); iter != mEdges.cend(); ++ iter) {
        const Edge* edge = *iter;
        edge->renderLines(mRenderer, mSlimeShader);
    }
}

void DesignerGameLayer::StraightEdge::renderVertices(const DeferredRenderer* mRenderer, const SlimeShader& mSlimeShader) const {
    
    SceneNode* mDummyPlate = new SceneNode();
    mDummyPlate->grab();
    mDummyPlate->setLocalTranslation(mPlate->mRenderLocation);
    mDummyPlate->setLocalOrientation(mPlate->mRenderOrientation);
    
    SceneNode* mUtilityNode = mDummyPlate->newChild();
    
    mUtilityNode->setLocalTranslation(mStartLoc);
    
    glUseProgram(mSlimeShader.mShaderProg->getHandle());
    
    glUniform3fv(mSlimeShader.mSunHandle, 1, glm::value_ptr(mRenderer->getSunDirection() * -1.f));
    glUniform3fv(mSlimeShader.mColorHandle, 1, glm::value_ptr(glm::vec3(1.f, 0.5f, 0.f)));
    mSlimeShader.mShaderProg->bindModelViewProjMatrices(mUtilityNode->calcWorldTransform(), mRenderer->getCameraViewMatrix(), mRenderer->getCameraProjectionMatrix());
    glBindVertexArray(mSlimeShader.mVertexBallVAO);
    mSlimeShader.mVertexBall->drawElements();
    
    
    glBindVertexArray(0);
    glUseProgram(0);
    
    mDummyPlate->drop();
    
}

void DesignerGameLayer::StraightEdge::renderLines(const DeferredRenderer* mRenderer, const SlimeShader& mSlimeShader) const {
    
    Vec3 disp = mEndLoc - mStartLoc;
    float length = disp.mag();
    Vec3 dir = disp / length;
    
    SceneNode* mDummyPlate = new SceneNode();
    mDummyPlate->grab();
    mDummyPlate->setLocalTranslation(mPlate->mRenderLocation);
    mDummyPlate->setLocalOrientation(mPlate->mRenderOrientation);
    
    SceneNode* mUtilityNode = mDummyPlate->newChild();
    
    mUtilityNode->setLocalTranslation(mStartLoc);
    mUtilityNode->setLocalScale(glm::vec3(1.f, 1.f, length));
    mUtilityNode->setLocalOrientation(quaternionLookAt(dir, Vec3(0.f, 0.f, 1.f), Vec3(0.f, 1.f, 0.f)));
    
    glUseProgram(mSlimeShader.mShaderProg->getHandle());
    
    glUniform3fv(mSlimeShader.mSunHandle, 1, glm::value_ptr(mRenderer->getSunDirection() * -1.f));
    
    if(mUnions.size() > 0) {
        glUniform3fv(mSlimeShader.mColorHandle, 1, glm::value_ptr(glm::vec3(0.f, 1.f, 1.f)));
    } else {
        glUniform3fv(mSlimeShader.mColorHandle, 1, glm::value_ptr(glm::vec3(1.f, 1.f, 0.f)));
    }
    
    mSlimeShader.mShaderProg->bindModelViewProjMatrices(mUtilityNode->calcWorldTransform(), mRenderer->getCameraViewMatrix(), mRenderer->getCameraProjectionMatrix());
    glBindVertexArray(mSlimeShader.mStraightEdgeVAO);
    mSlimeShader.mStraightEdge->drawElements();
    
    
    glBindVertexArray(0);
    glUseProgram(0);
    
    mDummyPlate->drop();
    
}

bool DesignerGameLayer::onWindowSizeUpdate(const WindowResizeEvent& event) {
    return true;
}
}

