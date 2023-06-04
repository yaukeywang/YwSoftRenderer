// Add by Yaukey at 2023-06-04.
// YW Soft Renderer demo with PBR-IBL camera class.

#ifndef __YW_DEMO_PBR_IBL_TEXTURED_CAMERA_H__
#define __YW_DEMO_PBR_IBL_TEXTURED_CAMERA_H__

#include "YwArcBallCamera.h"

namespace yw
{
    class DemoPBRIBLTexturedCamera : public ArcBallCamera
    {
    public:
        // Constructor.
        DemoPBRIBLTexturedCamera(class Graphics* graphics);

        // Destructor.
        ~DemoPBRIBLTexturedCamera();

    public:
        // Render a specified pass.
        // RenderPass sets the necessary states to render a specific pass and then calls the scene's render function.
        // -1 = render all passes.
        void RenderPass(int32_t pass = -1);
    };
}

#endif // !__YW_DEMO_PBR_IBL_TEXTURED_CAMERA_H__
