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

#ifndef PGG_DESIGNERPARTS_HPP
#define PGG_DESIGNERPARTS_HPP

#include <stdint.h>
#include <vector>

#include "btBulletDynamicsCommon.h"

#include "SceneNode.hpp"
#include "OpenGLStuff.hpp"
#include "DeferredRenderer.hpp"
#include "ShaderProgramResource.hpp"
#include "GeometryResource.hpp"
#include "Vec3.hpp"

namespace pgg {

struct SlimeShader {
    ShaderProgramResource* mShaderProg;
    GLuint mColorHandle;
    GLuint mSunHandle;
    
    GeometryResource* mVertexBall;
    GeometryResource* mStraightEdge;
    GeometryResource* mFlatSocket;
    GeometryResource* mOmniSocket;
    
    GLuint mVertexBallVAO;
    GLuint mStraightEdgeVAO;
    GLuint mFlatSocketVAO;
    GLuint mOmniSocketVAO;
};

class Plate;

class Edge {
public:
    enum Type {
        STRAIGHT,
        QUARTER
    };
    
    Edge(Type type, Plate* plate);
    virtual ~Edge();
    
    const Type mType;
    
    Plate* mPlate;
    
    std::vector<Edge*> mUnions;
    
    virtual bool canBindTo(Edge* other) const = 0;
    virtual void renderLines(const DeferredRenderer* renderer, const SlimeShader& mSlimeShader) const = 0;
    virtual void renderVertices(const DeferredRenderer* renderer, const SlimeShader& mSlimeShader) const = 0;
    
    virtual void onPlateChangeTransform(const Vec3& location, const glm::quat& orienation) = 0;
};

class StraightEdge : public Edge {
public:
    StraightEdge(Plate* plate, const Vec3& start, const Vec3& end);
    virtual ~StraightEdge();
    
    Vec3 mStartLoc;
    Vec3 mEndLoc;
    
    Vec3 mWorldStartLoc;
    Vec3 mWorldEndLoc;
    
    bool canBindTo(Edge* other) const;
    void renderLines(const DeferredRenderer* renderer, const SlimeShader& mSlimeShader) const;
    void renderVertices(const DeferredRenderer* renderer, const SlimeShader& mSlimeShader) const;
    
    void onPlateChangeTransform(const Vec3& location, const glm::quat& orientation);
};

class Socket {
public:
    enum Type {
        OMNI,
        FLAT,
        CURVED
    };
    
    Socket(Type type, Plate* plate);
    virtual ~Socket();
    
    const Type mType;
    
    Plate* mPlate;
    
    std::vector<Socket*> mUnions;
    
    virtual bool canBindTo(Socket* other) const = 0;
    virtual void render(const DeferredRenderer* renderer, const SlimeShader& mSlimeShader) const = 0;
    virtual void onPlateChangeTransform(const Vec3& location, const glm::quat& orientation) = 0;
};

class OmniSocket : public Socket {
public:
    OmniSocket(Plate* plate, const Vec3& location);
    virtual ~OmniSocket();
    
    Vec3 mLocation;
    
    Vec3 mWorldLocation;
    
    bool canBindTo(Socket* other) const;
    void render(const DeferredRenderer* renderer, const SlimeShader& mSlimeShader) const;
    void onPlateChangeTransform(const Vec3& location, const glm::quat& orientation);
};

class FlatSocket : public Socket {
public:
    FlatSocket(Plate* plate, const Vec3& location, const Vec3& normal);
    virtual ~FlatSocket();
    
    Vec3 mLocation;
    Vec3 mNormal;
    
    Vec3 mWorldLocation;
    Vec3 mWorldNormal;
    
    bool canBindTo(Socket* other) const;
    void render(const DeferredRenderer* render, const SlimeShader& mSlimeShader) const;
    void onPlateChangeTransform(const Vec3& location, const glm::quat& orienation);
};

class Plate {
public:
    Plate();
    virtual ~Plate();
    
    // Binding features
    std::vector<Edge*> mEdges;
    std::vector<Socket*> mSockets;
    
    // Precise storage of location
    int32_t mIntegralX;
    int32_t mIntegralY;
    int32_t mIntegralZ;
    
    // Precise storage of scale
    int32_t mIntegralScaleX;
    int32_t mIntegralScaleY;
    int32_t mIntegralScaleZ;
    
    // Imprecise storage of orienation
    Vec3 mLocation;
    glm::quat mOrientation;
    
    glm::quat mFinalizedOrienation;
    
    // Location and orientation as rendered
    Vec3 mRenderLocation;
    glm::quat mRenderOrientation;
    
    SceneNode* mSceneNode;
    btCollisionObject* collisionObject;
    btCollisionShape* collisionShape;
    btCollisionWorld* collisionWorld;
    btMotionState* motionState;
    
    Vec3 getLocation() const;
    void setLocation(Vec3 location, float snapSize);
    
    void onTransformChanged();
    bool needRebuildUnionGraph;
    void rebuildUnionGraph(std::vector<Plate*>& plates);
    
    float intermediatePitch;
    float intermediateYaw;
    float intermediateRoll;
    
    void setIntermediatePitch(float radians);
    void setIntermediateYaw(float radians);
    void setIntermediateRoll(float radians);
    void finalizeRotation();
    
    void renderEdges(const DeferredRenderer* mRenderer, const SlimeShader& mSlimeShader) const;
    void renderSockets(const DeferredRenderer* mRenderer, const SlimeShader& mSlimeShader) const;
    
    void tick(float tpf);
};
}

#endif // PGG_DESIGNERPARTS_HPP
