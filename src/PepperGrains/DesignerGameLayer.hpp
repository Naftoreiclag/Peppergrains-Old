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
    class Plate {
        Plate();
        ~Plate();
        
        uint32_t x;
        uint32_t y;
        uint32_t z;
        
        Vec3 transitionLocation;
    };
private:
    DeferredRenderer* mRenderer;
    
    InfiniteCheckerboardModel* mInfCheck;
    
    btRigidBody* mPlaneRigid;
    
    btBroadphaseInterface* mBroadphase;
    btDefaultCollisionConfiguration* mCollisionConfiguration;
    btCollisionDispatcher* mDispatcher;
    btSequentialImpulseConstraintSolver* mSolver;
    
    btDiscreteDynamicsWorld* mDynamicsWorld;
    
    SceneNodeESys* mSceneNodeESys;
    RigidBodyESys* mRigidBodyESys;
    
    SceneNode* mDebugCube;
    
    uint32_t mCube;
    
    uint32_t mScreenWidth;
    uint32_t mScreenHeight;
    
    Vec2 mMouseLoc;
    
    SceneNode* mRootNode;
    
    SceneNode* mCamLocNode;
    SceneNode* mCamYawNode;
    SceneNode* mCamPitchNode;
    SceneNode* mCamRollNode;

    float fps;
    float fpsWeight;
    float oneSecondTimer;
    
    bool mDebugWireframe;
    
    void renderFrame(glm::vec4 debugShow, bool wireframe);
public:
    DesignerGameLayer(uint32_t width, uint32_t height);
    ~DesignerGameLayer();
    
    // Lifecycle
    void onBegin();
    void onEnd();
    
    // Ticks
    void onTick(float tpf, const InputState* keyStates);
    bool onMouseMove(const MouseMoveEvent& event);
    
    bool onWindowSizeUpdate(const WindowResizeEvent& event);
    
};

}

#endif // DesignerGameLayer_HPP
