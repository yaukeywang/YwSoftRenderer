// Add by Yaukey at 2019-09-19.
// YW Soft Renderer demo with blinn-phong camera class.

#include "YwDemoBlinnPhongCamera.h"
#include "YwBaseApplication.h"
#include "YwGraphics.h"
#include "YwScene.h"

namespace yw
{
    DemoBlinnPhongCamera::DemoBlinnPhongCamera(Graphics* graphics) :
        Camera(graphics)
    {
    }

    DemoBlinnPhongCamera::~DemoBlinnPhongCamera()
    {

    }

    void DemoBlinnPhongCamera::RenderPass(int32_t pass)
    {
        Scene* scene = GetGraphics()->GetApplication()->GetScene();
        scene->Render(pass);
    }
}
