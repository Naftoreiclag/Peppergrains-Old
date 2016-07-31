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

#include "DesignerParts.hpp"
#include "ResourceManager.hpp"
#include "DeferredRenderer.hpp"
#include "Vec2.hpp"
#include "DebugInfiniteCheckerboardModel.hpp"
#include "Vec3.hpp"
#include "SoundEndpoint.hpp"
#include "SoundContext.hpp"
#include "SoundReceiver.hpp"

namespace pgg {

class DesignerGameLayer : public GameLayer {
private:
    DeferredRenderer* mRenderer;

    SlimeShader mSlimeShader;
    
    std::vector<Plate*> mPlates;
    
    InfiniteCheckerboardModel* mInfCheck;
    Sound::PlayingWaveformInterface* mPwi;
    double mQwerty;
    
    btRigidBody* mPlaneRigid;
    
    Sound::Endpoint* mSndEndpoint;
    Sound::Receiver* mSndReceiver;
    Sound::Context* mSndContext;
    
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
    
    bool mDebugShowX;
    bool mDebugShowY;
    bool mDebugShowZ;
    bool mDebugShowW;
    
    bool mDebugWireframe;
    bool mShowAllEdges;
    
    void newWheel(Vec3 location);
    void newConnector(Vec3 location);
    void newSquarePlate(Vec3 location);
    void newMotor(Vec3 location);
    void newCore(Vec3 location);
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
