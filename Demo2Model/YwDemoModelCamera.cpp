// Add by Yaukey at 2019-09-19.
// YW Soft Renderer demo triangle camera class.

#include "YwDemoModelCamera.h"
#include "YwBaseApplication.h"
#include "YwGraphics.h"
#include "YwScene.h"

namespace yw
{
    DemoModelCamera::DemoModelCamera(Graphics* graphics) :
        Camera(graphics)
    {
    }

    DemoModelCamera::~DemoModelCamera()
    {

    }

    void DemoModelCamera::RenderPass(int32_t pass)
    {
        Scene* scene = GetGraphics()->GetApplication()->GetScene();
        scene->Render(pass);
    }
}
