// Add by Yaukey at 2018-09-19.
// YW Soft Renderer scene class.

#ifndef __YW_SCENE_H__
#define __YW_SCENE_H__

#include "Yw3d.h"

namespace yw
{
    class Scene
    {
        friend class IApplication;

    protected:
        // Constructor.
        Scene(class IApplication* application);

        // Destructor.
        ~Scene();

    protected:
        // Initialize the scene.
        bool Initialize();

        // Each frame move.
        void FrameMove();

    public:
        // Get scene clear color.
        Vector4 GetClearColor() const;

        // Render with a specified pass.
        void Render(uint32_t pass);

    public:
        // Get parent application.
        inline class IApplication* GetApplication()
        {
            return m_Application;
        }

    private:
        // The parent application.
        class IApplication* m_Application;
    };
}

#endif // __YW_SCENE_H__
