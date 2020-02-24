// Add by Yaukey at 2020-02-24.
// YW Soft Renderer demo with PBR camera class.

#include "YwDemoPBRCamera.h"
#include "YwBaseApplication.h"
#include "YwGraphics.h"
#include "YwScene.h"

namespace yw
{
    DemoPBRCamera::DemoPBRCamera(Graphics* graphics) :
        Camera(graphics)
    {
    }

    DemoPBRCamera::~DemoPBRCamera()
    {

    }

    void DemoPBRCamera::RenderPass(int32_t pass)
    {
        Scene* scene = GetGraphics()->GetApplication()->GetScene();
        scene->Render(pass);
    }
}
