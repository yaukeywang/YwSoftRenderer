// Add by Yaukey at 2019-09-19.
// YW Soft Renderer demo model camera class.

#ifndef __YW_DEMO_MODEL_CAMERA_H__
#define __YW_DEMO_MODEL_CAMERA_H__

#include "YwCamera.h"

namespace yw
{
    class DemoModelCamera : public Camera
    {
    public:
        // Constructor.
        DemoModelCamera(class Graphics* graphics);

        // Destructor.
        ~DemoModelCamera();

    public:
        // Render a specified pass.
        // RenderPass sets the necessary states to render a specific pass and then calls the scene's render function.
        // -1 = render all passes.
        void RenderPass(int32_t pass = -1);
    };
}

#endif // !__YW_DEMO_MODEL_CAMERA_H__
