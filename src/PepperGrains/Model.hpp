#ifndef PGG_MODEL_HPP
#define PGG_MODEL_HPP

#include "ReferenceCounted.hpp"
#include "OpenGLStuff.hpp"

namespace pgg {

// Virtual inheritance to avoid diamond conflict with ModelResource
class Model : virtual public ReferenceCounted {
public:
    Model();
    virtual ~Model();

    virtual void render(const glm::mat4& viewMat, const glm::mat4& projMat, const glm::mat4& modelMat) = 0;
};

}

#endif // PGG_MODEL_HPP
