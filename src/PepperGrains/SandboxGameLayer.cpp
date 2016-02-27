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

#include "SandboxGameLayer.hpp"

#include <sstream>

namespace pgg
{

SandboxGameLayer::SandboxGameLayer()
{
}

SandboxGameLayer::~SandboxGameLayer()
{
}

// Lifecycle
void SandboxGameLayer::onBegin() {
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glClearColor(0.0f, 0.0f, 0.0f, 1.f);

    ResourceManager* resman = ResourceManager::getSingleton();

    rootNode = new SceneNode();
    friendNode = new SceneNode();
    
    rainstormFont = resman->findFont("Rainstorm.font");
    rainstormFont->grab();

    fpsCounter = new TextModel(rainstormFont, "FPS: Calculating...");
    fpsCounter->grab();

    friendNode->grabModel(resman->findModel("JellyUFO.model"));

    rootNode->addChild(friendNode);

    fps = 0.f;
    fpsWeight = 0.85f;

    oneSecondTimer = 0.f;
}
void SandboxGameLayer::onEnd() {
    
    fpsCounter->drop();


    rootNode->dropModel();
    
    rainstormFont->drop();
}

// Ticks
void SandboxGameLayer::onTick(float tpf, const Uint8* keyStates) {
    
    glm::mat4 viewMat = glm::lookAt(glm::vec3(0.f, 2.f, -2.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
    glm::mat4 projMat = glm::perspective(glm::radians(90.f), 1280.f / 720.f, 1.f, 10.f);

    glm::mat4 viewMatOverlay;
    glm::mat4 projMatOverlay = glm::ortho(0.f, (float) 1280, 0.f, (float) 720);
    glm::mat4 testMM;
    
    oneSecondTimer += tpf;


    if(tpf > 0) {
        float fpsNew = 1 / tpf;
        fps = (fps * fpsWeight) + (fpsNew * (1.f - fpsWeight));
    }

    if(oneSecondTimer > 1.f) {
        oneSecondTimer -= 1.f;

        fpsCounter->drop();

        std::stringstream ss;
        ss << "FPS: ";
        ss << (uint32_t) fps;

        fpsCounter = new TextModel(rainstormFont, ss.str());
        fpsCounter->grab();
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    rootNode->rotate(glm::vec3(0.0f, 1.0f, 0.0f), (float) tpf);
    rootNode->setLocalScale(glm::vec3(0.5f, 0.5f, 0.5f));
    friendNode->move(glm::vec3(0.f, 0.f, (float) (tpf * 0.3)));
    rootNode->render(viewMat, projMat);

    glClear(GL_DEPTH_BUFFER_BIT);

    fpsCounter->render(viewMatOverlay, projMatOverlay, testMM);
}

}

