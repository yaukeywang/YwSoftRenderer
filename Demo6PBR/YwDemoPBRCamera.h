// Add by Yaukey at 2020-02-24.
// YW Soft Renderer demo with PBR camera class.

#ifndef __YW_DEMO_PBR_CAMERA_H__
#define __YW_DEMO_PBR_CAMERA_H__

#include "YwArcBallCamera.h"

namespace yw
{
    class DemoPBRCamera : public ArcBallCamera
    {
    public:
        // Constructor.
        DemoPBRCamera(class Graphics* graphics);

        // Destructor.
        ~DemoPBRCamera();

    public:
        // Render a specified pass.
        // RenderPass sets the necessary states to render a specific pass and then calls the scene's render function.
        // -1 = render all passes.
        void RenderPass(int32_t pass = -1);
    };
}

#endif // !__YW_DEMO_PBR_CAMERA_H__
