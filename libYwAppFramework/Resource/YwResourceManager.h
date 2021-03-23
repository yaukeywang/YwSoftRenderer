// Add by Yaukey at 2019-06-03.
// YW Soft Renderer resource manager class.

#ifndef __YW_RESOURCE_MANAGER_H__
#define __YW_RESOURCE_MANAGER_H__

#include "YwBase.h"
#include <map>
#include <vector>

namespace yw
{
    // Define resource handle type.
    typedef uint32_t HRESOURCE;

    // Resources load and unload function types.
    typedef void* (*RESOURCELOADFUNCTION)(class ResourceManager* resourceManager, const StringA& filename);
    typedef void (*RESOURCEUNLOADFUNCTION)(class ResourceManager* resourceManager, void* resource);

    // Resource manager class.
    class ResourceManager
    {
        friend class IApplication;
        friend class Application;

    protected:
        // Constructor.
        ResourceManager(class IApplication* application);

        // Destructor.
        virtual ~ResourceManager();

    protected:
        // Initialize this system.
        bool Initialize();

    public:
        // Load a resource from file.
        HRESOURCE LoadResource(const StringA& fileName);

        // Unload a loaded resource.
        void UnloadResource(HRESOURCE hResource);

        // Get a loaded resource.
        void* GetResource(HRESOURCE hResource) const;

    protected:
        // Register a load/unload function for a resource extension.
        void RegisterResourceExtension(const StringA& extension, RESOURCELOADFUNCTION loadFunction, RESOURCEUNLOADFUNCTION unloadFunction);

        // Load and unload model file.
        static void* LoadModel(ResourceManager* resourceManager, const StringA& fileName);
        static void UnloadModel(ResourceManager* resourceManager, void* resource);

        // Load and unload "bmp" texture file.
        static void* LoadTexture_BMP(ResourceManager* resourceManager, const StringA& fileName);
        static void UnloadTexture_BMP(ResourceManager* resourceManager, void* resource);

        // Load and unload "png" texture file.
        static void* LoadTexture_PNG(ResourceManager* resourceManager, const StringA& fileName);
        static void UnloadTexture_PNG(ResourceManager* resourceManager, void* resource);

        // Load and unload "tga" texture file.
        static void* LoadTexture_TGA(ResourceManager* resourceManager, const StringA& fileName);
        static void UnloadTexture_TGA(ResourceManager* resourceManager, void* resource);

        static void* LoadTexture_HDR(ResourceManager* resourceManager, const StringA& fileName);
        static void UnloadTexture_HDR(ResourceManager* resourceManager, void* resource);

        // Load and unload "cube" texture file.
        static void* LoadTexture_Cube(ResourceManager* resourceManager, const StringA& fileName);
        static void UnloadTexture_Cube(ResourceManager* resourceManager, void* resource);

        // Load and unload "animated" texture file.
        static void* LoadTexture_Animated(ResourceManager* resourceManager, const StringA& fileName);
        static void UnloadTexture_Animated(ResourceManager* resourceManager, void* resource);

        // Get disk file path.
        StringA GetDiskFilePath(const StringA& fileName) const;

        // Get data root path.
        StringA GetDataPath() const;

    public:
        // Get parent application.
        inline class IApplication* GetApplication()
        {
            return m_Application;
        }

    protected:
        // The parent application.
        class IApplication* m_Application;

        // Registered load/unload entities for type of resource extension.
        std::map<StringA, RESOURCELOADFUNCTION> m_RegisteredEntityExtensionsLoad;
        std::map<StringA, RESOURCEUNLOADFUNCTION> m_RegisteredEntityExtensionsUnload;

        // Managed resource type.
        struct ManagedResource
        {
            HRESOURCE resourceHandle;
            uint32_t references;
            StringA fileName;
            StringA extension;
            void* resource;

            ManagedResource() : resourceHandle(0), references(0), resource(nullptr) {}
        };

        // All loaded managed resources.
        std::map<HRESOURCE, ManagedResource> m_ManagedResources;
        uint32_t m_NumberLoadedResources;
    };
}

#endif // !__YW_RESOURCE_MANAGER_H__
