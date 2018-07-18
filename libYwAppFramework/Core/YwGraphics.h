// Add by Yaukey at 2018-07-11.
// YW Soft Renderer 3d device graphics class.

#ifndef __YW_GRAPHICS_H__
#define __YW_GRAPHICS_H__

#include <stack>
#include "Yw3d.h"

namespace yw
{
    class Graphics
    {
        friend class IApplication;
        friend class Application;
        friend class StateBlock;
        friend class Camera;

    protected:
        Graphics(class IApplication* application);
        ~Graphics();

    protected:
        //bool Initialize(const struct CreationFlags& creationFlags);

    public:
        // Get object.
        inline Yw3d* GetYw3d()
        {
            return m_Yw3d;
        }

        // Get device.
        inline Yw3dDevice* GetYw3dDevice()
        {
            return m_Yw3dDevice;
        }

        // Get current camera.
        inline class Camera* GetCamera()
        {
            return m_Camera;
        }

        // Push current render state into stack.
        void PushStateBlock();

        // Pop current render state from stack.
        void PopStateBlock();

    protected:
        // Set current camera.
        inline void SetCamera(class Camera* camera)
        {
            m_Camera = camera;
        }

    protected:
        // Camera instance.
        class Camera* m_Camera;

    private:
        // Yw3d instance.
        Yw3d* m_Yw3d;

        // Yw3d device instance.
        Yw3dDevice* m_Yw3dDevice;

        // Base application class.
        class IApplication* m_Application;

        // All state block stack.
        std::stack<class StateBlock*> m_StateBlocks;
    };
}

#endif // !__YW_GRAPHICS_H__
