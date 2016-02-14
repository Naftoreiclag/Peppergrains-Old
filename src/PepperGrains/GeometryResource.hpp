#ifndef GEOMETRYRESOURCE_HPP
#define GEOMETRYRESOURCE_HPP

#include <stdint.h>

#include <OpenGLStuff.hpp>

#include "Resource.hpp"

namespace pgg {

class GeometryResource : public Resource {
private:
    struct Vertex {
        // Position
        float x;
        float y;
        float z;

        // Color
        float r;
        float g;
        float b;
        float a;

        // Texture
        float u;
        float v;

        // Normal
        float nx;
        float ny;
        float nz;

        Vertex()
        : x(0.f)
        , y(0.f)
        , z(0.f)
        , r(1.f)
        , g(1.f)
        , b(1.f)
        , a(1.f)
        , u(0.f)
        , v(0.f)
        , nx(1.f)
        , ny(0.f)
        , nz(0.f) {}
    };

    struct Triangle {
        uint32_t a;
        uint32_t b;
        uint32_t c;
    };

    typedef std::vector<Vertex> VertexBuffer;
    typedef std::vector<Triangle> TriangleBuffer;

    bool mUseColor;
    bool mUseUV;
    bool mUseNormals;
    bool mUsePositions;

    uint32_t mPositionOff;
    uint32_t mColorOff;
    uint32_t mTextureOff;
    uint32_t mNormalOff;
    uint32_t mSizeVertices;

    uint32_t mNumVertices;
    uint32_t mNumTriangles;

    GLuint mVertexBufferObject;
    GLuint mIndexBufferObject;

    bool mLoaded;
public:
    GeometryResource();
    virtual ~GeometryResource();

    bool load();
    bool unload();

    void render();
    void bindBuffers();
    void enablePositionAttrib(GLuint posAttrib);
    void enableColorAttrib(GLuint colorAttrib);
    void enableUVAttrib(GLuint textureAttrib);
    void enableNormalAttrib(GLuint normalAttrib);

    GLuint getHandle() const;
};

}

#endif // GEOMETRYRESOURCE_HPP
