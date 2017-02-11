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

#include "Video.hpp"
#include "ModelInstance.hpp"
#include "ModelResource.hpp"
#include "Resources.hpp"

namespace pgg {

MissionGameLayer::MissionGameLayer()
: mPeriod(0.f) {
}

MissionGameLayer::~MissionGameLayer()
{
}

// Lifecycle
void MissionGameLayer::onBegin() {
    mScenegraph = new SimpleScenegraph();
    
    mRenderer = new ShoRenderer();
    mRenderer->initialize();
    mRenderer->setScenegraph(mScenegraph);
    
    //mScenegraph->setModelInstance(new ModelInstance(ModelResource::gallop(Resources::find("CoolCactus.model"))));
    
    mRenderer->mCamera.setProjMatrix(glm::radians(50.f), Video::calcWindowAspectRatio(), 0.2f, 200.f);
    mRenderer->mCamera.setViewMatrix(glm::vec3(4.f, 4.f, -4.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
}
void MissionGameLayer::onEnd() {
    delete mScenegraph->setModelInstance(nullptr);
    delete mScenegraph;
    mRenderer->cleanup();
    delete mRenderer;
}


/*
bool MissionGameLayer::onQuit(const QuitEvent& event) {
    onEnd();
}
*/

// Ticks
void MissionGameLayer::onTick(double tpf, const InputState* keyStates) {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    
    //mRootNode->update(tpf);
    
    mPeriod += tpf;
    //mRootNode->mModelInst->mModelMatr = glm::rotate(glm::mat4(1.f), std::sin(mPeriod), glm::vec3(0.f, 1.f, 0.f));
    mRenderer->mCamera.setViewMatrix(glm::vec3(std::sin(mPeriod) * 4.f, 4.f, std::cos(mPeriod) * 4.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
    
    mRenderer->renderFrame();
}
bool MissionGameLayer::onNeedRebuildRenderPipeline() {
    mRenderer->mCamera.setProjMatrix(glm::radians(50.f), Video::calcWindowAspectRatio(), 0.2f, 200.f);
    mRenderer->rebuildPipeline();
    return false;
}

}

