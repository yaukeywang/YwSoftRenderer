// Add by Yaukey at 2020-01-23.
// YW Soft Renderer demo with normal-mapping camera class.

#ifndef __YW_DEMO_NORMAL_MAPPING_CAMERA_H__
#define __YW_DEMO_NORMAL_MAPPING_CAMERA_H__

#include "YwCamera.h"

namespace yw
{
    class DemoNormalMappingCamera : public Camera
    {
    public:
        // Constructor.
        DemoNormalMappingCamera(class Graphics* graphics);

        // Destructor.
        ~DemoNormalMappingCamera();

    public:
        // Render a specified pass.
        // RenderPass sets the necessary states to render a specific pass and then calls the scene's render function.
        // -1 = render all passes.
        void RenderPass(int32_t pass = -1);
    };
}

#endif // !__YW_DEMO_NORMAL_MAPPING_CAMERA_H__
