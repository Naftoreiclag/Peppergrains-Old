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

#ifndef PGG_SunLightModel_HPP
#define PGG_SunLightModel_HPP

#include "Model.hpp"
#include "ShaderProgramResource.hpp"

namespace pgg {

class SunLightModel : public Model {
public:
    class SharedResources : public ReferenceCounted {
    private:
        ShaderProgramResource* mMinimalShader;
        ShaderProgramResource* mShaderProg;
        GLuint mNormalHandle;
        GLuint mDepthHandle;
        GLuint mSunDepthHandle;
        GLuint mDirectionHandle;
        GLuint mColorHandle;
        
        GLuint mDLightVao;
        GLuint mDLightVbo;
        GLuint mDLightIbo;
        
        SharedResources();
        ~SharedResources();
        
    public:
        static SharedResources* getSharedInstance();
        
        void load();
        void unload();
        void render(const Model::RenderPassConfiguration& rendPass, const glm::mat4& modelMat, const glm::vec3& color);
    };
private:
    SharedResources* mSharedRes;
    
    glm::vec3 mColor;
public:
    SunLightModel(glm::vec3 color = glm::vec3(1.0));
    virtual ~SunLightModel();

    void load();
    void unload();

    void render(const Model::RenderPassConfiguration& rendPass, const glm::mat4& modelMat);
    
    void setColor(const glm::vec3& color);
};

}

#endif // PGG_SunLightModel_HPP
