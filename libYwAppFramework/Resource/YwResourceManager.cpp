// Add by Yaukey at 2019-06-03.
// YW Soft Renderer resource manager class.

#include "YwResourceManager.h"
#include "YwBaseApplication.h"
#include "YwGraphics.h"
#include "YwModel.h"
#include "YwModelLoaderWavefrontObj.h"
#include "YwTextureLoaderBmp.h"
#include "YwTextureLoaderPng.h"
#include "YwTextureLoaderTga.h"

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
            UnloadResource(m_ManagedResources.begin()->second.resourceHandle);
        }

        m_RegisteredEntityExtensionsLoad.clear();
        m_RegisteredEntityExtensionsUnload.clear();
    }

    bool ResourceManager::Initialize()
    {
        // Register resource file type by extension.
        RegisterResourceExtension("obj", LoadModel, UnloadModel);
        RegisterResourceExtension("bmp", LoadTexture_BMP, UnloadTexture_BMP);
        RegisterResourceExtension("png", LoadTexture_PNG, UnloadTexture_PNG);
        RegisterResourceExtension("tga", LoadTexture_TGA, UnloadTexture_TGA);
        RegisterResourceExtension("cube", LoadTexture_Cube, UnloadTexture_Cube);
        RegisterResourceExtension("anim", LoadTexture_Animated, UnloadTexture_Animated);

        return true;
    }

    HRESOURCE ResourceManager::LoadResource(const StringA& fileName)
    {
        // Find the exist resource.
        std::map<HRESOURCE, ManagedResource>::iterator resItr = m_ManagedResources.begin();
        for (; resItr != m_ManagedResources.end(); ++resItr)
        {
            if (resItr->second.fileName == fileName)
            {
                ++(resItr->second.references);
                return resItr->second.resourceHandle;
            }
        }

        // Not found, loading a new resource.
        const char* check = fileName.c_str();
        const char* ext = nullptr;
        while ('\0' != *check)
        {
            if ('.' == *check)
            {
                ext = check;
            }

            check++;
        }

        if (nullptr == ext)
        {
            return 0;
        }

        // Get extension name.
        StringA extension = ext + 1;

        // Get load function.
        RESOURCELOADFUNCTION loadFunc = m_RegisteredEntityExtensionsLoad[extension];
        if (nullptr == loadFunc)
        {
            return 0;
        }

        // Get file path on disk.
        StringA& filePath = GetDiskFilePath(fileName);

        // Load and create a resource.
        ManagedResource newResource;
        newResource.resource = loadFunc(this, filePath);
        if (nullptr == newResource.resource)
        {
            return 0;
        }

        newResource.resourceHandle = ++m_NumberLoadedResources;
        newResource.references = 1;
        newResource.fileName = fileName;
        newResource.extension = extension;
        m_ManagedResources.insert(std::pair<HRESOURCE, ManagedResource>(newResource.resourceHandle, newResource));

        return newResource.resourceHandle;
    }

    void ResourceManager::UnloadResource(HRESOURCE hResource)
    {
        if (0 == hResource)
        {
            return;
        }

        std::map<HRESOURCE, ManagedResource>::iterator itr = m_ManagedResources.find(hResource);
        if (m_ManagedResources.end() == itr)
        {
            return;
        }

        if (--(itr->second.references) == 0)
        {
            RESOURCEUNLOADFUNCTION unloadFunc = m_RegisteredEntityExtensionsUnload[itr->second.extension];
            if (nullptr != unloadFunc)
            {
                unloadFunc(this, itr->second.resource);
                itr->second.resource = nullptr;
            }

            m_ManagedResources.erase(itr);
        }
    }

    void* ResourceManager::GetResource(HRESOURCE hResource) const
    {
        if (0 == hResource)
        {
            return nullptr;
        }

        std::map<HRESOURCE, ManagedResource>::const_iterator itr = m_ManagedResources.find(hResource);
        if (m_ManagedResources.end() == itr)
        {
            return nullptr;
        }

        return itr->second.resource;
    }

    // Register a load/unload function for a resource extension.
    void ResourceManager::RegisterResourceExtension(const StringA& extension, RESOURCELOADFUNCTION loadFunction, RESOURCEUNLOADFUNCTION unloadFunction)
    {
        m_RegisteredEntityExtensionsLoad[extension] = loadFunction;
        m_RegisteredEntityExtensionsUnload[extension] = unloadFunction;
    }

    void* ResourceManager::LoadModel(ResourceManager* resourceManager, const StringA& fileName)
    {
        // Define a model.
        Model* model = nullptr;

        // Load model data by loader.
        ModelLoaderWavefrontObj modelLoader;
        if (!modelLoader.Load(fileName, resourceManager->GetApplication()->GetGraphics()->GetYw3dDevice(), &model, true, false, 90.0f, &fileName))
        {
            YW_SAFE_DELETE(model);
            return nullptr;
        }

        return model;
    }

    void ResourceManager::UnloadModel(ResourceManager* resourceManager, void* resource)
    {
        YW_SAFE_DELETE(resource);
    }

    void* ResourceManager::LoadTexture_BMP(ResourceManager* resourceManager, const StringA& fileName)
    {
        // Define a texture.
        Yw3dTexture* texture = nullptr;

        // Load texture data by loader.
        TextureLoaderBmp bmpLoader;
        if (!bmpLoader.Load(fileName, resourceManager->GetApplication()->GetGraphics()->GetYw3dDevice(), &texture, true))
        {
            YW_SAFE_RELEASE(texture);
            return nullptr;
        }

        return texture;
    }

    void ResourceManager::UnloadTexture_BMP(ResourceManager* resourceManager, void* resource)
    {
        Yw3dTexture* texture = (Yw3dTexture*)resource;
        YW_SAFE_RELEASE(texture);
    }

    void* ResourceManager::LoadTexture_PNG(ResourceManager* resourceManager, const StringA& fileName)
    {
        // Define a texture.
        Yw3dTexture* texture = nullptr;

        // Load texture data by loader.
        TextureLoaderPng pngLoader;
        if (!pngLoader.Load(fileName, resourceManager->GetApplication()->GetGraphics()->GetYw3dDevice(), &texture, true))
        {
            YW_SAFE_RELEASE(texture);
            return nullptr;
        }

        return texture;
    }

    void ResourceManager::UnloadTexture_PNG(ResourceManager* resourceManager, void* resource)
    {
        Yw3dTexture* texture = (Yw3dTexture*)resource;
        YW_SAFE_RELEASE(texture);
    }

    void* ResourceManager::LoadTexture_TGA(ResourceManager* resourceManager, const StringA& fileName)
    {
        // Define a texture.
        Yw3dTexture* texture = nullptr;

        // Load texture data by loader.
        TextureLoaderTga tgaLoader;
        if (!tgaLoader.Load(fileName, resourceManager->GetApplication()->GetGraphics()->GetYw3dDevice(), &texture, true))
        {
            YW_SAFE_RELEASE(texture);
            return nullptr;
        }

        return texture;
    }

    void ResourceManager::UnloadTexture_TGA(ResourceManager* resourceManager, void* resource)
    {
        Yw3dTexture* texture = (Yw3dTexture*)resource;
        YW_SAFE_RELEASE(texture);
    }

    void* ResourceManager::LoadTexture_Cube(ResourceManager* resourceManager, const StringA& fileName)
    {
        assert(nullptr && _T("LoadTexture_Cube is currently not supported!"));
        return nullptr;
    }

    void ResourceManager::UnloadTexture_Cube(ResourceManager* resourceManager, void* resource)
    {
        YW_SAFE_DELETE(resource);
    }

    void* ResourceManager::LoadTexture_Animated(ResourceManager* resourceManager, const StringA& fileName)
    {
        assert(nullptr && _T("LoadTexture_Animated is currently not supported!"));
        return nullptr;
    }

    void ResourceManager::UnloadTexture_Animated(ResourceManager* resourceManager, void* resource)
    {
        YW_SAFE_DELETE(resource);
    }

    StringA ResourceManager::GetDiskFilePath(const StringA& fileName) const
    {
        StringA diskFilePath = GetDataPath() + "/" + fileName;
        return diskFilePath;
    }

    StringA ResourceManager::GetDataPath() const
    {
#if defined(__amigaos4__) || (_AMIGAOS4)
        return StringA("Resources");
#else
        return StringA("./Resources");
#endif
    }
}
