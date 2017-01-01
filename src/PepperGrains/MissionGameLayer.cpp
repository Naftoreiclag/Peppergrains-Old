/*
   Copyright 2016-2017 James Fong

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

#include "MissionGamelayer.hpp"

#include <chrono>
#include <thread>
#include <math.h>

#include "ModelInstance.hpp"
#include "ModelResource.hpp"
#include "Resources.hpp"

namespace pgg {

MissionGameLayer::MissionGameLayer(uint32_t width, uint32_t height)
: mScreenWidth(width)
, mScreenHeight(height)
, mPeriod(0.f) {
}

MissionGameLayer::~MissionGameLayer()
{
}

// Lifecycle
void MissionGameLayer::onBegin() {
    mRenderer = new ShoRenderer(mScreenWidth, mScreenHeight);
    
    mRootNode = new SimpleScenegraph();
    mRenderer->mScenegraph = mRootNode;
    
    mRootNode->mModelInst = new ModelInstance(ModelResource::gallop(Resources::find("JellyCube.model")));
    
    mRenderer->mCamera.setProjMatrix(glm::radians(50.f), (float) mScreenWidth / (float) mScreenWidth, 0.2f, 200.f);
    mRenderer->mCamera.setViewMatrix(glm::vec3(4.f, 4.f, -4.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
}
void MissionGameLayer::onEnd() {
    
    delete mRootNode->mModelInst;
    delete mRootNode;
    delete mRenderer;
}

// Ticks
void MissionGameLayer::onTick(float tpf, const InputState* keyStates) {
    //std::this_thread::sleep_for(std::chrono::milliseconds(10));
    
    //mRootNode->update(tpf);
    
    mPeriod += tpf;
    mRootNode->mModelInst->mModelMatr = glm::rotate(glm::mat4(1.f), std::sin(mPeriod), glm::vec3(0.f, 1.f, 0.f));
    mRenderer->mCamera.setViewMatrix(glm::vec3(0.f, std::sin(mPeriod / 5) + 4.f, -4.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
    
    mRenderer->renderFrame();
}

}

