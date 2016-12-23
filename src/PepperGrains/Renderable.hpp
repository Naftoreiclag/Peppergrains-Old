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

#ifndef PGG_RENDERABLE_HPP
#define PGG_RENDERABLE_HPP

#include "OpenGLStuff.hpp"
#include "HardValueStuff.hpp"
#include "Camera.hpp"

namespace pgg {

class Renderable {
public:
    // Might someday have different configs for different renderables
    class Pass {
    public:
        enum Type {
            SHO_DEPTHPREPASS,
            SHO_SUNLIGHTIRRADIANCE,
            SHO_FORWARD,
            
            GEOMETRY,
            SHADOW,
            LOCAL_LIGHTS,
            GLOBAL_LIGHTS,
            SMAC_GEOMETRY,
            SSIPG,
            SCREEN
        };
        Type mType;
        
        Pass(Pass::Type renderPassType);
        virtual ~Pass();
        
        Camera mCamera;
        
        void calcFustrumAABB();
        
        glm::vec2 calcScreenSize();
        glm::vec2 calcInvScreenSize();
        
        uint32_t mScreenWidth;
        uint32_t mScreenHeight;
        
        float mCascadeBorders[PGG_NUM_SUN_CASCADES + 1];
        
        GLuint mFramebuffer;
        GLuint mDiffuseTexture;
        GLuint mNormalTexture;
        GLuint mBrightTexture;
        GLuint mDepthStencilTexture;
        
        GLuint mSunDepthTexture[PGG_NUM_SUN_CASCADES];
        glm::mat4 mSunViewProjMatr[PGG_NUM_SUN_CASCADES];
        
        bool mAvailableFustrumAABB;
        glm::vec3 mMinBB;
        glm::vec3 mMaxBB;
    };
    
    virtual void render(Renderable::Pass rendPass) = 0;
};
}

#endif // PGG_RENDERABLE_HPP
