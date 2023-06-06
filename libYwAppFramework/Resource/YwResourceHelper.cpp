// Add by Yaukey at 2023-06-06.
// YW Soft Renderer resource helper class.

#include "YwResourceHelper.h"

#include "YwModel.h"

namespace yw
{
    // Resource wrapper class for a single resource node.
    
    ResourceWrapper::ResourceWrapper(ResourceManager* resourceManager, StringA resourceFile, HRESOURCE resourceHandle, void* resourcePointer) :
        m_ResourceManager(resourceManager),
        m_ResourceFile(resourceFile),
        m_ResourceHandle(resourceHandle),
        m_ResourcePointer(resourcePointer)
    {
    }

    ResourceWrapper::~ResourceWrapper()
    {
        ReleaseResource();
    }

    void ResourceWrapper::ReleaseResource()
    {
        if ((0 == m_ResourceHandle) && (nullptr != m_ResourcePointer))
        {
            const StringA& fileExt = m_ResourceManager->GetFileExtension(m_ResourceFile);
            RESOURCEUNLOADFUNCTION unloadFunc = m_ResourceManager->GetResourceUnloaderByFileExtension(fileExt);
            if (nullptr != unloadFunc)
            {
                unloadFunc(m_ResourceManager, m_ResourcePointer);
            }
        }

        m_ResourcePointer = nullptr;

        if ((nullptr != m_ResourceManager) && (m_ResourceHandle > 0))
        {
            m_ResourceManager->UnloadResource(m_ResourceHandle);
            m_ResourceHandle = 0;
        }

        m_ResourceManager = nullptr;
    }

    // Resource helper class.
    
    ResourceWrapper* ResourceHelper::LoadResource(ResourceManager* resourceManager, const StringA& resourcePath)
    {
        if ((nullptr == resourceManager) || resourcePath.empty())
        {
            return nullptr;
        }
        
        HRESOURCE hResourceHandle = resourceManager->LoadResource(resourcePath);
        if (0 == hResourceHandle)
        {
            StringA errorMsg = "ResourceHelper.LoadResource: Load resource \"";
            errorMsg += resourcePath;
            errorMsg += "\" failed.";
            LOGE_A(errorMsg);
            
            return nullptr;
        }

        void* resource = resourceManager->GetResource(hResourceHandle);
        if (nullptr == resource)
        {
            StringA errorMsg = "ResourceHelper.LoadResource: Get resource \"";
            errorMsg += resourcePath;
            errorMsg += "\" failed.";
            LOGE_A(errorMsg);
            
            return nullptr;
        }

        ResourceWrapper* resourceWrapper = new ResourceWrapper(resourceManager, resourcePath, hResourceHandle, resource);
        return resourceWrapper;
    }
}
