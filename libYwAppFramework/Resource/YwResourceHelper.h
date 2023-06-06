// Add by Yaukey at 2023-06-06.
// YW Soft Renderer resource helper class.

#ifndef __YW_RESOURCE_HELPER_H__
#define __YW_RESOURCE_HELPER_H__

#include "YwResourceManager.h"

namespace yw
{
    // Resource wrapper class for a single resource node.
    class ResourceWrapper
    {
    public:
        ResourceWrapper(ResourceManager* resourceManager, StringA resourceFile, HRESOURCE resourceHandle, void* resourcePointer);
        ~ResourceWrapper();

    public:
        template<typename T>
        inline T* GetResource() const
        {
            return static_cast<T*>(m_ResourcePointer);
        }

        void ReleaseResource();

    private:
        ResourceManager* m_ResourceManager;
        StringA m_ResourceFile;
        HRESOURCE m_ResourceHandle;
        void* m_ResourcePointer;
    };

    // Resource helper class.
    class ResourceHelper
    {
    public:
        static ResourceWrapper* LoadResource(ResourceManager* resourceManager, const StringA& resourcePath);
    };
}

#endif // !__YW_RESOURCE_HELPER_H__
