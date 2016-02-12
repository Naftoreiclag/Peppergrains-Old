#ifndef MODELRESOURCE_HPP
#define MODELRESOURCE_HPP

#include "GeometryResource.hpp"
#include "MaterialResource.hpp"
#include "Resource.hpp" // Base class: Resource

class ModelResource : public Resource {
private:
    bool mLoaded;

    GeometryResource* mGeometry;
    MaterialResource* mMaterial;
    GLuint mVertexArrayObject;
public:
    ModelResource();
    virtual ~ModelResource();

public:
    bool load();
    bool unload();
    void render(const glm::mat4& viewMat, const glm::mat4& projMat, const glm::mat4& modelMat);
};

#endif // MODELRESOURCE_HPP
