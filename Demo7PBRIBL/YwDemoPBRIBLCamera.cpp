// Add by Yaukey at 2020-04-18.
// YW Soft Renderer demo with PBR-IBL camera class.

#include "YwDemoPBRIBLCamera.h"
#include "YwBaseApplication.h"
#include "YwGraphics.h"
#include "YwScene.h"

namespace yw
{
    DemoPBRIBLCamera::DemoPBRIBLCamera(Graphics* graphics) :
        ArcBallCamera(graphics)
    {
    }

    DemoPBRIBLCamera::~DemoPBRIBLCamera()
    {

    }

    void DemoPBRIBLCamera::RenderPass(int32_t pass)
    {
        Scene* scene = GetGraphics()->GetApplication()->GetScene();
        scene->Render(pass);
    }
}
