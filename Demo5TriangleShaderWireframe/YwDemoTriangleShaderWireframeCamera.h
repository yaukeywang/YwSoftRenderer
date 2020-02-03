// Add by Yaukey at 2020-02-02.
// YW Soft Renderer demo with triangle shader wireframe camera class.

#ifndef __YW_DEMO_TRIANGLE_SHADER_WIREFRAME_CAMERA_H__
#define __YW_DEMO_TRIANGLE_SHADER_WIREFRAME_CAMERA_H__

#include "YwCamera.h"

namespace yw
{
    class DemoTriangleShaderWireframeCamera : public Camera
    {
    public:
        // Constructor.
        DemoTriangleShaderWireframeCamera(class Graphics* graphics);

        // Destructor.
        ~DemoTriangleShaderWireframeCamera();

    public:
        // Render a specified pass.
        // RenderPass sets the necessary states to render a specific pass and then calls the scene's render function.
        // -1 = render all passes.
        void RenderPass(int32_t pass = -1);
    };
}

#endif // !__YW_DEMO_TRIANGLE_SHADER_WIREFRAME_CAMERA_H__
