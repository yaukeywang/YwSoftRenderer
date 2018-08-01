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
        YW_SAFE_RELEASE(m_RenderTarget);
    }

    bool Camera::CreateRenderCamera(uint32_t width, uint32_t height, Yw3dFormat frameBufferFormat, bool enableDepthBuffer, bool enableStencilBuffer)
    {
        if (m_LockedSurfacesViewport)
        {
            return false;
        }

        // Get 3d device.
        Yw3dDevice* device = m_Graphics->GetYw3dDevice();

        // Create the render texture.
        Yw3dSurface* colorBuffer = nullptr;
        if (YW3D_FAILED(device->CreateSurface(&colorBuffer, width, height, frameBufferFormat)))
        {
            return false;
        }

        // Create the depth texture.
        Yw3dSurface* depthBuffer = nullptr;
        if (enableDepthBuffer)
        {
            if (YW3D_FAILED(device->CreateSurface(&depthBuffer, width, height, Yw3d_FMT_R32F)))
            {
                YW_SAFE_RELEASE(colorBuffer);
                return false;
            }
        }

        // Set the viewport.
        Matrix44 matViewport;
        Matrix44Viewport(matViewport, 0, 0, width, height, 0.0f, 1.0f);
        m_RenderTarget->SetViewportMatrix(matViewport);

        // Set color buffer and depth buffer.
        m_RenderTarget->SetColorBuffer(colorBuffer);
        m_RenderTarget->SetDepthBuffer(depthBuffer);

        // Decrease the references of buffer.
        YW_SAFE_RELEASE(colorBuffer);
        YW_SAFE_RELEASE(depthBuffer);

        // Don't allow any more changes to surfaces/viewport!
        m_LockedSurfacesViewport = true;

        return true;
    }

    void Camera::CalculateProjection(float fov, float aspect, float zNear, float zFar)
    {
        // Calculating the projection matrix.
        Matrix44PerspectiveFovLH(m_ProjectionMatrix, fov, aspect, zNear, zFar);

        // Remember the parameters.
        m_FovAngle = fov;
        m_Aspect = aspect;
        m_NearClippingPlane = zNear;
        m_FarClippingPlane = zFar;
    }

    void Camera::CalculateView()
    {
        // Note: Matrix44LookAtLH already gives the view matrix.

        // Get translation matrix.
        Matrix44 matTranslation;
        Matrix44Translation(matTranslation, -m_Position);

        // Get rotation matrix.
        Matrix44 matRotation;
        Matrix44FromQuaternion(matRotation, m_Rotation);

        // Get combined view matrix, this is equal from result of Matrix44LookAtLH.
        m_ViewMatrix = matTranslation * matRotation;

        // Update axis vector.
        m_Forward.Set(m_ViewMatrix._13, m_ViewMatrix._23, m_ViewMatrix._33);
        m_Right.Set(m_ViewMatrix._11, m_ViewMatrix._21, m_ViewMatrix._31);
        m_Up.Set(m_ViewMatrix._12, m_ViewMatrix._22, m_ViewMatrix._32);

        // Re-Build frustum.
        BuildFrustum();
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

    void Camera::BuildFrustum()
    {
        // Calculating frustum planes.
        Matrix44 matViewProjection = GetViewMatrix() * GetProjectionMatrix();

        // Left: -w' < x'.
        m_Frustum[Yw3d_CP_Left] = Plane(
            matViewProjection._14 + matViewProjection._11,
            matViewProjection._24 + matViewProjection._21,
            matViewProjection._34 + matViewProjection._31,
            matViewProjection._44 + matViewProjection._41
        );

        // Right: x' < w';
        m_Frustum[Yw3d_CP_Right] = Plane(
            matViewProjection._14 - matViewProjection._11,
            matViewProjection._24 - matViewProjection._21,
            matViewProjection._34 - matViewProjection._31,
            matViewProjection._44 - matViewProjection._41
        );

        // Top: y' < w'.
        m_Frustum[Yw3d_CP_Top] = Plane(
            matViewProjection._14 - matViewProjection._12,
            matViewProjection._24 - matViewProjection._22,
            matViewProjection._34 - matViewProjection._32,
            matViewProjection._44 - matViewProjection._42
        );

        // Bottom: -w' < y'.
        m_Frustum[Yw3d_CP_Bottom] = Plane(
            matViewProjection._14 + matViewProjection._12,
            matViewProjection._24 + matViewProjection._22,
            matViewProjection._34 + matViewProjection._32,
            matViewProjection._44 + matViewProjection._42
        );

        // Near: 0 < z'.
        m_Frustum[Yw3d_CP_Near] = Plane(
            matViewProjection._13,
            matViewProjection._23,
            matViewProjection._33,
            matViewProjection._43
        );

        // Far: z' < w'.
        m_Frustum[Yw3d_CP_Far] = Plane(
            matViewProjection._14 - matViewProjection._13,
            matViewProjection._24 - matViewProjection._23,
            matViewProjection._34 - matViewProjection._33,
            matViewProjection._44 - matViewProjection._43
        );

        // Normalize all frustum planes.
        for (int i = 0; i < Yw3d_CP_NumFrustumPlanes; i++)
        {
            m_Frustum[i].normal.Normalize();
        }
    }
}
