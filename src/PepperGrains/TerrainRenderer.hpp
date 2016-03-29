#ifndef PGG_TERRAINRENDERER_HPP
#define PGG_TERRAINRENDERER_HPP

#include "OpenGLStuff.hpp"

namespace pgg {

class TerrainRenderer {
public:
    TerrainRenderer();
    ~TerrainRenderer();
    
    void render(glm::mat4 viewMatr, glm::mat4 projMatr);
};

}

#endif // PGG_TERRAINRENDERER_HPP
