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

#ifndef DesignerGameLayer_HPP
#define DesignerGameLayer_HPP

#include "GameLayer.hpp" // Base class: pgg::GameLayer

#include <vector>

#include "btBulletDynamicsCommon.h"
#include "SDL2/SDL.h"

#include "NRES.hpp"

#include "AxesModel.hpp"
#include "ResourceManager.hpp"
#include "RigidBodyESys.hpp"
#include "SceneNode.hpp"
#include "SceneNodeESys.hpp"
#include "SunLightModel.hpp"
#include "TerrainModel.hpp"
#include "TerrainRenderer.hpp"
#include "TextModel.hpp"
#include "TextureResource.hpp"
#include "DebugInfiniteCheckerboardModel.hpp"
#include "DeferredRenderer.hpp"
#include "Vec2.hpp"

namespace pgg {

class DesignerGameLayer : public GameLayer {
public:
    struct SlimeShader {
        ShaderProgramResource* mShaderProg;
        GLuint mColorHandle;
        GLuint mSunHandle;
        
        GeometryResource* mVertexBall;
        GeometryResource* mStraightEdge;
        
        GLuint mVertexBallVAO;
        GLuint mStraightEdgeVAO;
    };
    
    SlimeShader mSlimeShader;
    
    class Plate;
    class Edge {
    public:
        enum Type {
            STRAIGHT
        };
        
        Edge(Type type, Plate* plate);
        virtual ~Edge();
        
        const Type mType;
        
        Plate* mPlate;
        
        Vec3 mLocation;
        
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
        
        void onPlateChangeTransform(const Vec3& location, const glm::quat& orienation);
    };
    
    class Plate {
    public:
        Plate();
        virtual ~Plate();
        
        std::vector<Edge*> mEdges;
        
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
        
        void tick(float tpf);
    };
    
    
    
private:
    DeferredRenderer* mRenderer;
    
    std::vector<Plate*> mPlates;
    
    InfiniteCheckerboardModel* mInfCheck;
    
    btRigidBody* mPlaneRigid;
    
    struct CollisionWorldPackage {
        CollisionWorldPackage();
        ~CollisionWorldPackage();
        
        btBroadphaseInterface* mBroadphase;
        btDefaultCollisionConfiguration* mCollisionConfiguration;
        btCollisionDispatcher* mDispatcher;
        btCollisionWorld* mCollisionWorld;
    };
    CollisionWorldPackage mCollisionPackage[2];
    btCollisionWorld* mCollisionWorld;
    btCollisionWorld* mManipulatorCollisionWorld;
    
    SceneNodeESys* mSceneNodeESys;
    RigidBodyESys* mRigidBodyESys;
    
    struct Manipulator {
        Manipulator();
        ~Manipulator();
        
        GeometryResource* arrow;
        GeometryResource* wheel;
        
        GLuint arrowVAO;
        GLuint wheelVAO;
        
        Vec3 location;
        float scale;
        
        float initialAxisDragFrac;
        Vec2 initialWheelDragVector;
        
        int8_t handleHovered;
        int8_t handleDragged;
        
        btCollisionObject* collisionObjects[6];
        btCollisionShape* collisionShapes[6];
        btMotionState* motionStates[6];
        
        
        Vec3 xArrowLoc;
        Vec3 yArrowLoc;
        Vec3 zArrowLoc;
        Vec3 xWheelLoc;
        Vec3 yWheelLoc;
        Vec3 zWheelLoc;
    };
    
    Manipulator mManipulator;
    
    void loadSlimeShader();
    void unloadSlimeShader();
    void updateManipulatorTransform();
    void updateManipulatorPhysics();
    void loadManipulator();
    void unloadManipulator();
    void renderSecondLayer();
    
    float mCameraSpeed;
    float mCameraSpeedMin;
    float mCameraAcceleration;
    float mCameraSpeedMax;
    
    int32_t mGridSize;
    
    Plate* mPlateHovered;
    Plate* mPlateSelected;
    Plate* mPlateFreeDragged;
    Vec3 mPlateDragPoint;
    float mDragPlaneDistance;
    
    SceneNode* mDebugCube;
    
    uint32_t mCube;
    
    uint32_t mScreenWidth;
    uint32_t mScreenHeight;
    
    bool mMouseLeftDownLastFrame;
    Vec2 mMouseLoc;
    
    SceneNode* mRootNode;
    
    SceneNode* mCamLocNode;
    SceneNode* mCamYawNode;
    SceneNode* mCamPitchNode;
    SceneNode* mCamRollNode;

    float cyclicSawtooth;
    float cyclicSinusodal;

    float fps;
    float fpsWeight;
    float oneSecondTimer;
    
    bool mDebugWireframe;
    
    void newPlate();
    void deletePlate(Plate* plate);
    
    void selectPlate(Plate* plate);
    void deselectPlate();
    
    void renderFrame(glm::vec4 debugShow, bool wireframe);
public:
    DesignerGameLayer(uint32_t width, uint32_t height);
    ~DesignerGameLayer();
    
    // Lifecycle
    void onBegin();
    void onEnd();
    
    // Ticks
    void onTick(float tpf, const InputState* keyStates);
    
    bool onWindowSizeUpdate(const WindowResizeEvent& event);
    
};

}

#endif // DesignerGameLayer_HPP
