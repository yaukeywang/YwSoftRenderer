// Add by Yaukey at 2018-07-11.
// YW Soft Renderer camera class.

#include "YwCamera.h"
#include "YwGraphics.h"

namespace yw
{
    Camera::Camera(Graphics* graphics) : 
        m_Graphics(graphics),
        m_LockedSurfacesViewport(false),
        m_FovAngle(90.0f),
        m_Aspect(1.0f),
        m_NearClippingPlane(0.1f),
        m_FarClippingPlane(1.0f)
    {
        if (YW3D_FAILED(m_Graphics->GetYw3dDevice()->CreateRenderTarget(&m_RenderTarget)))
        {
            m_RenderTarget = nullptr;
        }

        // Identity all matrices.
        Matrix44Identity(m_WorldMatrix);
        Matrix44Identity(m_ViewMatrix);
        Matrix44Identity(m_ProjectionMatrix);

        // Build frustum.
        BuildFrustum();
    }

    Camera::~Camera()
    {
    }

    bool Camera::CreateRenderCamera(uint32_t width, uint32_t height, Yw3dFormat frameBuffer, bool depthBuffer, bool stencilBuffer)
    {
        return false;
    }

    void Camera::CalculateProjection(float fov, float aspect, float zNear, float zFar)
    {

    }

    void Camera::CalculateView()
    {

    }

    Camera::Visibility Camera::CheckSphereVisible(const Vector3& origin, float radius)
    {
        return Camera::Visibility_CompleteOut;
    }

    Camera::Visibility Camera::CheckBoxVisible(const Vector3& lower, const Vector3& upper)
    {
        return Camera::Visibility_CompleteOut;
    }

    void Camera::ClearToSceneColor(const Yw3dRect* rect)
    {

    }

    void Camera::BeginRender()
    {

    }

    void Camera::EndRender(bool presentToScreen)
    {

    }
}
