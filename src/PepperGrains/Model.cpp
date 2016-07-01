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

#include "Model.hpp"

namespace pgg {
Model::Model() { }
Model::~Model() { }
Model::RenderPass::RenderPass(RenderPass::Type renderPassType)
: type(renderPassType)
, availableFustrumAABB(false) {
}

void Model::RenderPass::setScreenSize(uint32_t width, uint32_t height) {
    screenSize = glm::vec2((float) width, (float) height);
    invScreenSize = glm::vec2(1.f / ((float) width), 1.f / ((float) height));
}

Model::RenderPass::~RenderPass() { }
void Model::RenderPass::calculateFustrumAABB() {
    glm::mat4 invVPMatrix = glm::inverse(projMat * viewMat);

    for(uint8_t j = 0; j < 8; ++ j) {
        glm::vec4 corner(
            (j & (1 << 0)) ? -1.f : 1.f,
            (j & (1 << 1)) ? -1.f : 1.f,
            (j & (1 << 2)) ? -1.f : 1.f,
            1.f
        );

        glm::vec4 cornerWorldSpace = invVPMatrix * corner;
        cornerWorldSpace /= cornerWorldSpace.w; // Perspective divide

        // cornerWorldSpace is a coordinate in world space for this corner of the view fustrum

        if(j == 0) {
            minBB = glm::vec3(cornerWorldSpace);
            maxBB = glm::vec3(cornerWorldSpace);
        } else {
            if(cornerWorldSpace.x < minBB.x) { minBB.x = cornerWorldSpace.x; }
            if(cornerWorldSpace.y < minBB.y) { minBB.y = cornerWorldSpace.y; }
            if(cornerWorldSpace.z < minBB.z) { minBB.z = cornerWorldSpace.z; }
            if(cornerWorldSpace.x > maxBB.x) { maxBB.x = cornerWorldSpace.x; }
            if(cornerWorldSpace.y > maxBB.y) { maxBB.y = cornerWorldSpace.y; }
            if(cornerWorldSpace.z > maxBB.z) { maxBB.z = cornerWorldSpace.z; }
        }
    }
    
    availableFustrumAABB = true;
}
}
