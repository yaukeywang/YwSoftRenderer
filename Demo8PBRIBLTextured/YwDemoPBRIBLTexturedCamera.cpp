// Add by Yaukey at 2022-06-04.
// YW Soft Renderer demo with PBR-IBL-Textured camera class.

#include "YwDemoPBRIBLTexturedCamera.h"
#include "YwBaseApplication.h"
#include "YwGraphics.h"
#include "YwScene.h"

namespace yw
{
    DemoPBRIBLTexturedCamera::DemoPBRIBLTexturedCamera(Graphics* graphics) :
        ArcBallCamera(graphics)
    {
    }

    DemoPBRIBLTexturedCamera::~DemoPBRIBLTexturedCamera()
    {

    }

    void DemoPBRIBLTexturedCamera::RenderPass(int32_t pass)
    {
        Scene* scene = GetGraphics()->GetApplication()->GetScene();
        scene->Render(pass);
    }
}
