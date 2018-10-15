// Add by Yaukey at 2018-10-10.
// YW Soft Renderer demo triangle camera class.

#ifndef __YW_DEMO_TRIANGLE_CAMERA_H__
#define __YW_DEMO_TRIANGLE_CAMERA_H__

#include "YwCamera.h"

namespace yw
{
    class DemoTriangleCamera : public Camera
    {
    public:
        // Constructor.
        DemoTriangleCamera(class Graphics* graphics);

        // Destructor.
        ~DemoTriangleCamera();

    public:
        // Render a specified pass.
        // RenderPass sets the necessary states to render a specific pass and then calls the scene's render function.
        // -1 = render all passes.
        void RenderPass(int32_t pass = -1);
    };
}

#endif // !__YW_DEMO_TRIANGLE_CAMERA_H__
