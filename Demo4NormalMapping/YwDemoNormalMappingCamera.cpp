// Add by Yaukey at 2020-01-23.
// YW Soft Renderer demo with normal-mapping camera class.

#include "YwDemoNormalMappingCamera.h"
#include "YwBaseApplication.h"
#include "YwGraphics.h"
#include "YwScene.h"

namespace yw
{
    DemoNormalMappingCamera::DemoNormalMappingCamera(Graphics* graphics) :
        Camera(graphics)
    {
    }

    DemoNormalMappingCamera::~DemoNormalMappingCamera()
    {

    }

    void DemoNormalMappingCamera::RenderPass(int32_t pass)
    {
        Scene* scene = GetGraphics()->GetApplication()->GetScene();
        scene->Render(pass);
    }
}
