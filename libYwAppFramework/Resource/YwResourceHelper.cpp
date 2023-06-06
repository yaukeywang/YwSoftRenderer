// Add by Yaukey at 2023-06-06.
// YW Soft Renderer resource helper class.

#include "YwResourceHelper.h"

namespace yw
{
    // Resource wrapper class for a single resource node.
    
    ResourceWrapper::ResourceWrapper(ResourceManager* ResourceManager, HRESOURCE ResourceHandle, void* ResourcePointer) :
        m_ResourceManager(ResourceManager),
        m_ResourceHandle(ResourceHandle),
        m_ResourcePointer(ResourcePointer)
    {
    }

    ResourceWrapper::~ResourceWrapper()
    {
        ReleaseResource();
    }

    void ResourceWrapper::ReleaseResource()
    {
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
        if (hResourceHandle <= 0)
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

        ResourceWrapper* resourceWrapper = new ResourceWrapper(resourceManager, hResourceHandle, resource);
        return resourceWrapper;
    }
}
