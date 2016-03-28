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

#ifndef PGG_INSTANCED_MODEL
#define PGG_INSTANCED_MODEL

#include "Model.hpp"
#include "GeometryResource.hpp"
#include "ShaderProgramResource.hpp"
#include "OpenGLStuff.hpp"

namespace pgg {

class InstancedModel : public Model {
private:
    GLuint mVertexArrayObject;
    GLuint mVertexBufferObject;
    GLuint mIndexBufferObject;
    ShaderProgramResource* mShaderProg;
    GeometryResource* mGeometry;
    GLuint mOffsetHandle;
    GLuint mInstanceBuffer;
public:
    InstancedModel();
    virtual ~InstancedModel();

    void load();
    void unload();

    void render(const Model::RenderPassConfiguration& rendPass, const glm::mat4& modelMat);

};

}


#endif // PGG_INSTANCED_MODEL