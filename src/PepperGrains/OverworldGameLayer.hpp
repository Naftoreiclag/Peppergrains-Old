/*
   Copyright 2015-2016 James Fong

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

#ifndef OverworldGameLayer_HPP
#define OverworldGameLayer_HPP

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

namespace pgg {

class OverworldGameLayer : public GameLayer {
private:
    struct GBuffer {
        GLuint gFramebuffer;
        
        GLuint diffuseTexture;
        GLuint normalTexture;
        GLuint brightTexture;
        GLuint depthStencilTexture;
    };
    GBuffer mGBuff;

    struct ScreenShader {
        ShaderProgramResource* shaderProg;
        
        GLuint diffuseHandle;
        GLuint brightHandle;
    };
    ScreenShader mScreenShader;
    
    struct DebugScreenShader {
        ShaderProgramResource* shaderProg;
        
        GLuint diffuseHandle;
        GLuint normalHandle;
        GLuint depthHandle;
        GLuint brightHandle;
        
        GLuint viewHandle;
    };
    DebugScreenShader mDebugScreenShader;
    
    struct DebugSunShader {
        ShaderProgramResource* shaderProg;
        
        GLuint mapHandles[PGG_NUM_SUN_CASCADES];
        
        GLuint viewHandle;
    };
    DebugSunShader mDebugSunShader;
    
    struct SkyStencilShader {
        ShaderProgramResource* shaderProg;
    };
    SkyStencilShader mSkyStencilShader;
    
    struct DebugScreenFillShader {
        ShaderProgramResource* shaderProg;
        GLuint colorHandle;
    };
    DebugScreenFillShader mFillScreenShader;
    
    struct Camera {
        float fov;
        float aspect;
        float nearDepth;
        float farDepth;
        
        float cascadeBorders[PGG_NUM_SUN_CASCADES + 1];
        
        glm::mat4 viewMat;
        glm::mat4 projMat;
        glm::vec3 position;
    };
    Camera mCamera;
    
    struct Sky {
        uint32_t sunTextureSize;
        glm::vec3 sunDirection;
        glm::vec3 sunPosition;
        
        // Todo: just swap out depth textures for each framebuffer
        GLuint sunFramebuffers[PGG_NUM_SUN_CASCADES];
        GLuint sunDepthTextures[PGG_NUM_SUN_CASCADES];
        
        glm::mat4 sunProjMatrices[PGG_NUM_SUN_CASCADES];
        glm::mat4 sunViewMatrices[PGG_NUM_SUN_CASCADES];
        
        glm::mat4 sunBasicViewMatrix;
        
        SunLightModel* sunModel;
    };
    Sky mSky;
    
    nres::World* mEntityWorld;
    
    nres::Entity* mPlayerEntity;
    
    SceneNode* mTestCubes[24];
    
    InfiniteCheckerboardModel* mInfCheck;
    
    btRigidBody* mPlaneRigid;
    
    btBroadphaseInterface* mBroadphase;
    btDefaultCollisionConfiguration* mCollisionConfiguration;
    btCollisionDispatcher* mDispatcher;
    btSequentialImpulseConstraintSolver* mSolver;
    
    btDiscreteDynamicsWorld* mDynamicsWorld;
    
    SceneNodeESys* mSceneNodeESys;
    RigidBodyESys* mRigidBodyESys;
    
    ShaderProgramResource* mComputer;
    TextureResource* mRoseTexture;
    
    TerrainModel* mTerrainModel;
    
    uint32_t mScreenWidth;
    uint32_t mScreenHeight;
    
    void loadGBuffer();
    void unloadGBuffer();
    void loadSun();
    void unloadSun();
    
    void renderFrame(glm::vec4 debugShow, bool wireframe);
public:
    OverworldGameLayer(uint32_t width, uint32_t height);
    ~OverworldGameLayer();
    
    SceneNode* mRootNode;
    
    SceneNode* mCamLocNode;
    SceneNode* mCamYawNode;
    SceneNode* mCamPitchNode;
    SceneNode* mCamRollNode;
    
    GLuint mFullscreenVao;
    GLuint mFullscreenVbo;
    GLuint mFullscreenIbo;
    
    FontResource* rainstormFont;
    TextModel* fpsCounter;
    TerrainModel* testTerrain;

    float fps;
    float fpsWeight;

    float oneSecondTimer;
    
    bool mDebugWireframe;
    
    // Lifecycle
    void onBegin();
    void onEnd();
    
    // Ticks
    void onTick(float tps, const Uint8* keyStates);
    bool onMouseMove(const SDL_MouseMotionEvent& event);
    
    bool onWindowSizeUpdate(const SDL_WindowEvent& event);
    
};

}

#endif // OverworldGameLayer_HPP
