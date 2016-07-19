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

#include "DesignerParts.hpp"
#include "MathUtil.hpp"

#include <iostream>

namespace pgg
{

Edge::Edge(Type type, Plate* plate)
: mType(type)
, mPlate(plate) { }
Edge::~Edge() { }

StraightEdge::StraightEdge(Plate* plate, const Vec3& start, const Vec3& end)
: Edge(Edge::Type::STRAIGHT, plate)
, mStartLoc(start)
, mEndLoc(end) { }
StraightEdge::~StraightEdge() { }

bool StraightEdge::canBindTo(Edge* otherEdge) const {
    assert(otherEdge != this && "Attempting to bind straight edge to itself!");
    assert(otherEdge->mPlate != mPlate && "Attempting to bind two edges of the same plate!");
    if(otherEdge->mType != Edge::Type::STRAIGHT) {
        return false;
    }
    StraightEdge* other = static_cast<StraightEdge*>(otherEdge);
    
    return Math::lineSegmentsColinear(mWorldStartLoc, mWorldEndLoc, other->mWorldStartLoc, other->mWorldEndLoc);
}
void StraightEdge::renderLines(const DeferredRenderer* mRenderer, const SlimeShader& mSlimeShader) const {
    
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
    mUtilityNode->setLocalOrientation(Math::quaternionLookAt(dir, Vec3(0.f, 0.f, 1.f), Vec3(0.f, 1.f, 0.f)));
    
    glUseProgram(mSlimeShader.mSunAwareProg->getHandle());
    
    glUniform3fv(mSlimeShader.mSunAwareSunDirectionHandle, 1, glm::value_ptr(mRenderer->getSunDirection() * -1.f));
    if(mLinks.size() > 0) {
        glUniform3fv(mSlimeShader.mSunAwareColorHandle, 1, glm::value_ptr(glm::vec3(0.f, 1.f, 1.f)));
    } else {
        glUniform3fv(mSlimeShader.mSunAwareColorHandle, 1, glm::value_ptr(glm::vec3(1.f, 1.f, 0.f)));
    }
    glBlendColor(0.4f, 0.4f, 0.4f, 1.f);
    mSlimeShader.mSunAwareProg->bindModelViewProjMatrices(mUtilityNode->calcWorldTransform(), mRenderer->getCameraViewMatrix(), mRenderer->getCameraProjectionMatrix());
    glBindVertexArray(mSlimeShader.mStraightEdgeVAO);
    mSlimeShader.mStraightEdge->drawElements();
    
    
    glBindVertexArray(0);
    glUseProgram(0);
    
    mDummyPlate->drop();
    
}
void StraightEdge::renderVertices(const DeferredRenderer* mRenderer, const SlimeShader& mSlimeShader) const {
    
    SceneNode* mDummyPlate = new SceneNode();
    mDummyPlate->grab();
    mDummyPlate->setLocalTranslation(mPlate->mRenderLocation);
    mDummyPlate->setLocalOrientation(mPlate->mRenderOrientation);
    
    SceneNode* mUtilityNode = mDummyPlate->newChild();
    
    mUtilityNode->setLocalTranslation(mStartLoc);
    
    glUseProgram(mSlimeShader.mSunAwareProg->getHandle());
    
    glUniform3fv(mSlimeShader.mSunAwareSunDirectionHandle, 1, glm::value_ptr(mRenderer->getSunDirection() * -1.f));
    glUniform3fv(mSlimeShader.mSunAwareColorHandle, 1, glm::value_ptr(glm::vec3(1.f, 0.f, 1.f)));
    glBlendColor(0.4f, 0.4f, 0.4f, 1.f);
    mSlimeShader.mSunAwareProg->bindModelViewProjMatrices(mUtilityNode->calcWorldTransform(), mRenderer->getCameraViewMatrix(), mRenderer->getCameraProjectionMatrix());
    glBindVertexArray(mSlimeShader.mVertexBallVAO);
    mSlimeShader.mVertexBall->drawElements();
    
    
    glBindVertexArray(0);
    glUseProgram(0);
    
    mDummyPlate->drop();
    
}
void StraightEdge::onPlateChangeTransform(const Vec3& location, const glm::quat& orientation) {
    mWorldStartLoc = Vec3(glm::mat4_cast(orientation) * glm::vec4(glm::vec3(mStartLoc), 1.f));
    mWorldEndLoc = Vec3(glm::mat4_cast(orientation) * glm::vec4(glm::vec3(mEndLoc), 1.f));
    
    mWorldStartLoc += location;
    mWorldEndLoc += location;
}

Socket::Socket(Type type, Plate* plate)
: mType(type)
, mPlate(plate) { }
Socket::~Socket() { }

OmniSocket::OmniSocket(Plate* plate, const Vec3& location)
: Socket(Socket::Type::OMNI, plate)
, mLocation(location){ }
OmniSocket::~OmniSocket() { }

bool OmniSocket::canBindTo(Socket* otherSocket) const {
    assert(otherSocket != this && "Attempting to bind omni socket to itself!");
    assert(otherSocket->mPlate != mPlate && "Attempting to bind two sockets of the same plate!");
    if(otherSocket->mType == Socket::Type::OMNI) {
        OmniSocket* other = static_cast<OmniSocket*>(otherSocket);
        return Math::equalish(other->mWorldLocation, mWorldLocation);
    } else if(otherSocket->mType == Socket::Type::FLAT) {
        FlatSocket* other = static_cast<FlatSocket*>(otherSocket);
        return Math::equalish(other->mWorldLocation, mWorldLocation);
    }
}
void OmniSocket::render(const DeferredRenderer* renderer, const SlimeShader& mSlimeShader) const {
    SceneNode* mDummyPlate = new SceneNode();
    mDummyPlate->grab();
    mDummyPlate->setLocalTranslation(mPlate->mRenderLocation);
    mDummyPlate->setLocalOrientation(mPlate->mRenderOrientation);
    SceneNode* mUtilityNode = mDummyPlate->newChild();
    mUtilityNode->setLocalTranslation(mLocation);
    
    glUseProgram(mSlimeShader.mSunAwareProg->getHandle());
    
    glUniform3fv(mSlimeShader.mSunAwareSunDirectionHandle, 1, glm::value_ptr(renderer->getSunDirection() * -1.f));
    if(mLinks.size() > 0) {
        glUniform3fv(mSlimeShader.mSunAwareColorHandle, 1, glm::value_ptr(glm::vec3(0.f, 0.5f, 1.f)));
    } else {
        glUniform3fv(mSlimeShader.mSunAwareColorHandle, 1, glm::value_ptr(glm::vec3(1.f, 0.5f, 0.f)));
    }
    glBlendColor(0.4f, 0.4f, 0.4f, 1.f);
    mSlimeShader.mSunAwareProg->bindModelViewProjMatrices(mUtilityNode->calcWorldTransform(), renderer->getCameraViewMatrix(), renderer->getCameraProjectionMatrix());
    glBindVertexArray(mSlimeShader.mOmniSocketVAO);
    mSlimeShader.mOmniSocket->drawElements();
    
    glBindVertexArray(0);
    glUseProgram(0);
    
    mDummyPlate->drop();
}
void OmniSocket::onPlateChangeTransform(const Vec3& location, const glm::quat& orientation) {
    mWorldLocation = Vec3(glm::mat4_cast(orientation) * glm::vec4(glm::vec3(mLocation), 1.f));
    
    mWorldLocation += location;
}

FlatSocket::FlatSocket(Plate* plate, const Vec3& location, const Vec3& normal, bool renderBothSides)
: Socket(Socket::Type::FLAT, plate)
, mLocation(location)
, mNormal(normal)
, mRenderBothSides(renderBothSides) { }
FlatSocket::~FlatSocket() { }

bool FlatSocket::canBindTo(Socket* otherSocket) const {
    assert(otherSocket != this && "Attempting to bind flat socket to itself!");
    assert(otherSocket->mPlate != mPlate && "Attempting to bind two sockets of the same plate!");
    if(otherSocket->mType == Socket::Type::OMNI) {
        OmniSocket* other = static_cast<OmniSocket*>(otherSocket);
        return Math::equalish(other->mWorldLocation, mWorldLocation);
    } else if(otherSocket->mType == Socket::Type::FLAT) {
        FlatSocket* other = static_cast<FlatSocket*>(otherSocket);
        if(!Math::equalish(other->mWorldLocation, mWorldLocation)) {
            return false;
        }
        return Math::oppositeDirection(other->mWorldNormal, mWorldNormal);
    }
    
}
void FlatSocket::render(const DeferredRenderer* renderer, const SlimeShader& mSlimeShader) const {
    SceneNode* mDummyPlate = new SceneNode();
    mDummyPlate->grab();
    mDummyPlate->setLocalTranslation(mPlate->mRenderLocation);
    mDummyPlate->setLocalOrientation(mPlate->mRenderOrientation);
    SceneNode* mUtilityNode = mDummyPlate->newChild();
    mUtilityNode->setLocalTranslation(mLocation);
    mUtilityNode->setLocalOrientation(Math::quaternionLookAt(Vec3(mNormal), Vec3(0.f, 0.f, 1.f), Vec3(0.f, 1.f, 0.f)));
    
    glUseProgram(mSlimeShader.mSunUnawareProg->getHandle());
    
    if(mLinks.size() > 0) {
        glUniform3fv(mSlimeShader.mSunUnawareColorHandle, 1, glm::value_ptr(glm::vec3(0.f, 0.5f, 1.f)));
    } else {
        glUniform3fv(mSlimeShader.mSunUnawareColorHandle, 1, glm::value_ptr(glm::vec3(1.f, 0.5f, 0.f)));
    }
    glBlendColor(0.4f, 0.4f, 0.4f, 1.f);
    mSlimeShader.mSunUnawareProg->bindModelViewProjMatrices(mUtilityNode->calcWorldTransform(), renderer->getCameraViewMatrix(), renderer->getCameraProjectionMatrix());
    glBindVertexArray(mSlimeShader.mFlatSocketVAO);
    
    if(mRenderBothSides) {
        glDisable(GL_CULL_FACE);
    }
    mSlimeShader.mFlatSocket->drawElements();
    if(mRenderBothSides) {
        glEnable(GL_CULL_FACE);
    }
    
    glBindVertexArray(0);
    glUseProgram(0);
    
    mDummyPlate->drop();
}
void FlatSocket::onPlateChangeTransform(const Vec3& location, const glm::quat& orientation) {
    mWorldLocation = Vec3(glm::mat4_cast(orientation) * glm::vec4(glm::vec3(mLocation), 1.f));
    mWorldNormal = Vec3(glm::mat4_cast(orientation) * glm::vec4(glm::vec3(mNormal), 0.f));
    
    mWorldLocation += location;
}

Plate::Plate()
: mIntegralX(0)
, mIntegralY(0)
, mIntegralZ(0)
, mIntegralScaleX(1)
, mIntegralScaleY(1)
, mIntegralScaleZ(1)
, intermediatePitch(0.f)
, intermediateYaw(0.f)
, intermediateRoll(0.f)
, needRebuildLinks(false)
, mAllowManualLocation(true)
, mAllowManualRotation(true)
, mIsConnector(false)
, mIsMotor(false) { }
Plate::~Plate() { }

void Plate::setIntermediatePitch(float radians) {
    intermediateYaw = 0;
    intermediateRoll = 0;
    float newRot = Math::nearestRightAngle(radians);
    if(intermediatePitch != newRot) {
        intermediatePitch = newRot;
        mOrientation = glm::angleAxis(intermediatePitch, glm::vec3(1.f, 0.f, 0.f)) * mFinalizedOrienation;
        onTransformChanged();
    }
}
void Plate::setIntermediateYaw(float radians) {
    intermediatePitch = 0;
    intermediateRoll = 0;
    float newRot = Math::nearestRightAngle(radians);
    if(intermediateYaw != newRot) {
        intermediateYaw = newRot;
        mOrientation = glm::angleAxis(intermediateYaw, glm::vec3(0.f, 1.f, 0.f)) * mFinalizedOrienation;
        onTransformChanged();
    }
}
void Plate::setIntermediateRoll(float radians) {
    intermediatePitch = 0;
    intermediateYaw = 0;
    float newRot = Math::nearestRightAngle(radians);
    if(intermediateRoll != newRot) {
        intermediateRoll = newRot;
        mOrientation = glm::angleAxis(intermediateRoll, glm::vec3(0.f, 0.f, 1.f)) * mFinalizedOrienation;
        onTransformChanged();
    }
}
void Plate::finalizeRotation() {
    mFinalizedOrienation = mOrientation;
    
    intermediatePitch = 0.f;
    intermediateYaw = 0.f;
    intermediateRoll = 0.f;
}
Vec3 Plate::getLocation() const {
    return Vec3(
        ((float) mIntegralX) * (1.f / 12.f),
        ((float) mIntegralY) * (1.f / 12.f),
        ((float) mIntegralZ) * (1.f / 12.f)
    );
}
void Plate::onTransformChanged() {
    needRebuildLinks = true;
    
    for(std::vector<Edge*>::iterator iter2 = mEdges.begin(); iter2 != mEdges.end(); ++ iter2) {
        Edge* myEdge = *iter2;
        myEdge->onPlateChangeTransform(mLocation, mOrientation);
    }
    for(std::vector<Socket*>::iterator iter2 = mSockets.begin(); iter2 != mSockets.end(); ++ iter2) {
        Socket* mySocket = *iter2;
        mySocket->onPlateChangeTransform(mLocation, mOrientation);
    }
}
void Plate::rebuildLinks(std::vector<Plate*>& plates) {
    if(!needRebuildLinks) {
        return;
    }

    for(std::vector<Edge*>::iterator myEdgesIterator = mEdges.begin(); myEdgesIterator != mEdges.end(); ++ myEdgesIterator) {
        Edge* myEdge = *myEdgesIterator;

        // Disconnect from all edges
        for(std::vector<Edge*>::iterator myEdgesLinksIterator = myEdge->mLinks.begin(); myEdgesLinksIterator != myEdge->mLinks.end(); ++ myEdgesLinksIterator) {
            Edge* otherEdge = *myEdgesLinksIterator;
            otherEdge->mLinks.erase(std::remove(otherEdge->mLinks.begin(), otherEdge->mLinks.end(), myEdge), otherEdge->mLinks.end());
        }
        myEdge->mLinks.clear();
        
        // Connect (or reconnect) to edges that are valid
        for(std::vector<Plate*>::iterator otherPlateIterator = plates.begin(); otherPlateIterator != plates.end(); ++ otherPlateIterator) {
            Plate* other = *otherPlateIterator;
            
            // Skip self
            if(this == other) { continue; }
            
            // Skip multiplate structure partners
            bool isPartner = false;
            for(std::vector<Plate*>::iterator linkedPlatesIterator = mMultiplatePartners.begin(); linkedPlatesIterator != mMultiplatePartners.end(); ++ linkedPlatesIterator) {
                Plate* partner = *linkedPlatesIterator;
                
                if(partner == other) {
                    isPartner = true;
                    break;
                }
            }
            if(isPartner) { continue; }
        
            for(std::vector<Edge*>::iterator otherPlateEdgesIterator = other->mEdges.begin(); otherPlateEdgesIterator != other->mEdges.end(); ++ otherPlateEdgesIterator) {
                Edge* otherEdge = *otherPlateEdgesIterator;
                assert(myEdge != otherEdge && "Two plates share the same edge instance!");
                if(myEdge->canBindTo(otherEdge) && otherEdge->canBindTo(myEdge)) {
                    myEdge->mLinks.push_back(otherEdge);
                    otherEdge->mLinks.push_back(myEdge);
                }
            }
        }
    }
    
    for(std::vector<Socket*>::iterator mySocketsIterator = mSockets.begin(); mySocketsIterator != mSockets.end(); ++ mySocketsIterator) {
        Socket* mySocket = *mySocketsIterator;

        // Disconnect from all Sockets
        for(std::vector<Socket*>::iterator mySocketsLinksIterator = mySocket->mLinks.begin(); mySocketsLinksIterator != mySocket->mLinks.end(); ++ mySocketsLinksIterator) {
            Socket* otherSocket = *mySocketsLinksIterator;
            otherSocket->mLinks.erase(std::remove(otherSocket->mLinks.begin(), otherSocket->mLinks.end(), mySocket), otherSocket->mLinks.end());
        }
        mySocket->mLinks.clear();
        
        // Connect (or reconnect) to sockets that are valid
        for(std::vector<Plate*>::iterator otherPlateIterator = plates.begin(); otherPlateIterator != plates.end(); ++ otherPlateIterator) {
            Plate* other = *otherPlateIterator;
            
            // Skip self
            if(this == other) { continue; }
            
            // Skip multiplate structure partners
            bool isPartner = false;
            for(std::vector<Plate*>::iterator linkedPlatesIterator = mMultiplatePartners.begin(); linkedPlatesIterator != mMultiplatePartners.end(); ++ linkedPlatesIterator) {
                Plate* partner = *linkedPlatesIterator;
                
                if(partner == other) {
                    isPartner = true;
                    break;
                }
            }
            if(isPartner) { continue; }
        
            for(std::vector<Socket*>::iterator otherPlateSocketsIterator = other->mSockets.begin(); otherPlateSocketsIterator != other->mSockets.end(); ++ otherPlateSocketsIterator) {
                Socket* otherSocket = *otherPlateSocketsIterator;
                assert(mySocket != otherSocket && "Two plates share the same socket instance!");
                if(mySocket->canBindTo(otherSocket) && otherSocket->canBindTo(mySocket)) {
                    mySocket->mLinks.push_back(otherSocket);
                    otherSocket->mLinks.push_back(mySocket);
                }
            }
        }
    }
    
    needRebuildLinks = false;
}
void Plate::setLocation(Vec3 location, float snapSize) {
    uint32_t newIntegralX = (uint32_t) std::floor(Math::nearestMultiple(location.x, snapSize) * 12.f + 0.5);
    uint32_t newIntegralY = (uint32_t) std::floor(Math::nearestMultiple(location.y, snapSize) * 12.f + 0.5);
    uint32_t newIntegralZ = (uint32_t) std::floor(Math::nearestMultiple(location.z, snapSize) * 12.f + 0.5);
    
    if(mIntegralX != newIntegralX || mIntegralY != newIntegralY || mIntegralZ != newIntegralZ) {
        mIntegralX = newIntegralX;
        mIntegralY = newIntegralY;
        mIntegralZ = newIntegralZ;
        mLocation = Vec3((float) mIntegralX, (float) mIntegralY, (float) mIntegralZ);
        mLocation /= 12.f;
        onTransformChanged();
    }
}
void Plate::tick(float tpf) {
    
    if((mLocation - mRenderLocation).mag() < tpf) {
        mRenderLocation = mLocation;
    } else {
        mRenderLocation += (mLocation - mRenderLocation) * tpf * 15.f;
    }
    
    mRenderOrientation = glm::slerp(mRenderOrientation, mOrientation, tpf * 15.f);
    
    mSceneNode->setLocalTranslation(mRenderLocation);
    mSceneNode->setLocalOrientation(mRenderOrientation);
    
    if(mIsConnector) {
        assert(mMultiplatePartners.begin() != mMultiplatePartners.end() && "Connector without multiplate partner!");
        
        if(mConnectorData.isChief) {
            Plate* mOtherPlate = *(mMultiplatePartners.begin());
            
            Vec3 worldStart = Vec3(mSceneNode->calcWorldTransform() * glm::vec4(0.f, 1.f / 12.f, 0.f, 1.f));
            Vec3 worldEnd = Vec3(mOtherPlate->mSceneNode->calcWorldTransform() * glm::vec4(0.f, 1.f / 12.f, 0.f, 1.f));
            
            Vec3 displacement = worldEnd - worldStart;
            float magnitude = displacement.mag();
            
            mConnectorData.pipe->setLocalTranslation(worldStart);
            mConnectorData.pipe->setLocalOrientation(Math::quaternionLookAt(
                displacement / magnitude, 
                Vec3(0.f, 0.f, 1.f),
                Vec3(0.f, 1.f, 0.f)));
            mConnectorData.pipe->setLocalScale(Vec3(1.f, 1.f, magnitude));
        }
    }
    
    // TODO: only run this when links change
    if(mIsMotor) {
        uint8_t i = 0;
        for(std::vector<Socket*>::iterator iter = mSockets.begin(); iter != mSockets.end(); ++ iter) {
            Socket* socket = *iter;
            
            if(socket->mLinks.size() > 0) {
                mMotorData.struts[i]->setVisible(true);
            } else {
                mMotorData.struts[i]->setVisible(false);
            }
            
            ++ i;
            
            if(i >= 4) {
                break;
            }
        }
    }
    
    mCollisionObject->getWorldTransform().setOrigin(mRenderLocation);
    mCollisionObject->getWorldTransform().setRotation(Quate(mRenderOrientation));
    mCollisionWorld->removeCollisionObject(mCollisionObject);
    mCollisionWorld->addCollisionObject(mCollisionObject);
}
void Plate::renderEdges(const DeferredRenderer* mRenderer, const SlimeShader& mSlimeShader) const {
    for(std::vector<Edge*>::const_iterator iter = mEdges.cbegin(); iter != mEdges.cend(); ++ iter) {
        const Edge* edge = *iter;
        edge->renderVertices(mRenderer, mSlimeShader);
    }
    for(std::vector<Edge*>::const_iterator iter = mEdges.cbegin(); iter != mEdges.cend(); ++ iter) {
        const Edge* edge = *iter;
        edge->renderLines(mRenderer, mSlimeShader);
    }
}
void Plate::renderSockets(const DeferredRenderer* mRenderer, const SlimeShader& mSlimeShader) const {
    for(std::vector<Socket*>::const_iterator iter = mSockets.cbegin(); iter != mSockets.cend(); ++ iter) {
        const Socket* socket = *iter;
        socket->render(mRenderer, mSlimeShader);
    }
}


}

