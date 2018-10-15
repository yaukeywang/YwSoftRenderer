// Add by Yaukey at 2018-10-10.
// YW Soft Renderer demo triangle camera class.

#include "YwDemoTriangleCamera.h"
#include "YwBaseApplication.h"
#include "YwGraphics.h"
#include "YwScene.h"

namespace yw
{
    DemoTriangleCamera::DemoTriangleCamera(Graphics* graphics) :
        Camera(graphics)
    {
    }

    DemoTriangleCamera::~DemoTriangleCamera()
    {

    }

    void DemoTriangleCamera::RenderPass(int32_t pass)
    {
        Scene* scene = GetGraphics()->GetApplication()->GetScene();
        scene->Render(pass);
    }
}
