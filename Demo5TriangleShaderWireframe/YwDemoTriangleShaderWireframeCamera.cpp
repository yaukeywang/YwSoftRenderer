// Add by Yaukey at 2020-01-23.
// YW Soft Renderer demo with triangle shader wireframe camera class.

#include "YwDemoTriangleShaderWireframeCamera.h"
#include "YwBaseApplication.h"
#include "YwGraphics.h"
#include "YwScene.h"

namespace yw
{
    DemoTriangleShaderWireframeCamera::DemoTriangleShaderWireframeCamera(Graphics* graphics) :
        Camera(graphics)
    {
    }

    DemoTriangleShaderWireframeCamera::~DemoTriangleShaderWireframeCamera()
    {

    }

    void DemoTriangleShaderWireframeCamera::RenderPass(int32_t pass)
    {
        Scene* scene = GetGraphics()->GetApplication()->GetScene();
        scene->Render(pass);
    }
}
