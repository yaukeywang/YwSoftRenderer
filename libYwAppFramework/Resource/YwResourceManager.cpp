// Add by Yaukey at 2019-06-03.
// YW Soft Renderer resource manager class.

#include "YwResourceManager.h"
#include "YwBaseApplication.h"

namespace yw
{
    ResourceManager::ResourceManager(IApplication* application) :
        m_Application(application),
        m_NumberLoadedResources(0)
    {
    }

    ResourceManager::~ResourceManager()
    {
        while (m_ManagedResources.size() > 0)
        {
            UnloadResource(m_ManagedResources.begin()->resourceHandle);
        }
    }

    bool ResourceManager::Initialize()
    {
        // Register resource file type by extension.
        RegisterResourceExtension("obj", LoadModel, UnloadModel);
        RegisterResourceExtension("tga", LoadTexture_TGA, UnloadTexture_TGA);
        RegisterResourceExtension("png", LoadTexture_PNG, UnloadTexture_PNG);
        RegisterResourceExtension("cube", LoadCubeTexture, UnloadCubeTexture);
        RegisterResourceExtension("anim", LoadAnimatedTexture, UnloadAnimatedTexture);

        return true;
    }

    // Register a load/unload function for a resource extension.
    void ResourceManager::RegisterResourceExtension(const StringA& extension, RESOURCELOADFUNCTION loadFunction, RESOURCEUNLOADFUNCTION unloadFunction)
    {
        m_RegisteredEntityExtensionsLoad[extension] = loadFunction;
        m_RegisteredEntityExtensionsUnload[extension] = unloadFunction;
    }
}
