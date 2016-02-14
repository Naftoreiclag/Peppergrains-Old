#ifndef RESOURCEMANAGER_HPP
#define RESOURCEMANAGER_HPP

#include <map>

#include <boost/filesystem.hpp>

#include "ImageResource.hpp"
#include "MaterialResource.hpp"
#include "MiscResource.hpp"
#include "ModelResource.hpp"
#include "Resource.hpp"
#include "StringResource.hpp"
#include "TextureResource.hpp"
#include "GeometryResource.hpp"
#include "ShaderResource.hpp"
#include "ShaderProgramResource.hpp"
#include "VertexShaderResource.hpp"
#include "FragmentShaderResource.hpp"

namespace pgg {

class ResourceManager {
public:
    static ResourceManager* getSingleton();
private:
    std::map<std::string, StringResource*> mStrings;
    std::map<std::string, ImageResource*> mImages;
    std::map<std::string, TextureResource*> mTextures;
    std::map<std::string, ModelResource*> mModels;
    std::map<std::string, MaterialResource*> mMaterials;
    std::map<std::string, GeometryResource*> mGeometries;
    std::map<std::string, ShaderResource*> mShaders;
    std::map<std::string, ShaderProgramResource*> mShaderPrograms;
    std::map<std::string, MiscResource*> mMiscs;

    uint32_t mPermaloadThreshold;

public:
    ResourceManager();
    ~ResourceManager();

    void setPermaloadThreshold(uint32_t size);
    const uint32_t& getPermaloadThreshold();

    void mapAll(boost::filesystem::path data);

    StringResource* findString(std::string name);
    ImageResource* findImage(std::string name);
    TextureResource* findTexture(std::string name);
    ModelResource* findModel(std::string name);
    MaterialResource* findMaterial(std::string name);
    GeometryResource* findGeometry(std::string name);
    ShaderResource* findShader(std::string name);
    ShaderProgramResource* findShaderProgram(std::string name);
};

}

#endif // RESOURCEMANAGER_HPP


