// Add by Yaukey at 2018-07-11.
// YW Soft Renderer camera class.

#ifndef __YW_CAMERA_H__
#define __YW_CAMERA_H__

#include "Yw3d.h"

namespace yw
{
    class Camera
    {
    public:
        // Constructor.
        Camera(class Graphics* graphics);

        // Destructor.
        virtual ~Camera();

    public:
        // Create rendersurface/depthsurface/stencilsurface and replaces set ones+viewport with them -> after calling this you can't change surfaces anymore!
        bool CreateRenderCamera(uint32_t width, uint32_t height, Yw3dFormat frameBuffer = Yw3d_FMT_R32G32B32F, bool depthBuffer = true, bool stencilBuffer = false);

        // Call this before calling CalculateView(), because the projection matrix is needed for frustum calcs!
        void CalculateProjection(float fov, float aspect = 4.0f / 3.0f, float zNear = 0.1f, float zFar = 1.0f);

        // Has to be called after making changes to camera position / rotation.
        void CalculateView();

    public:
        // Get parent of graphics.
        inline class Graphics* GetGraphics() const
        {
            return m_Graphics;
        }

        // Get render target.
        inline Yw3dRenderTarget* GetRenderTarget() const
        {
            return m_RenderTarget;
        }

        // Get world matrix.
        inline const Matrix44& GetWorldMatrix() const
        {
            return m_WorldMatrix;
        }

        // Set world matrix.
        inline void SetWorldMatrix(const Matrix44& worldMatrix)
        {
            m_WorldMatrix = worldMatrix;
        }

        // Get view matrix.
        inline const Matrix44& GetViewMatrix() const
        {
            return m_ViewMatrix;
        }

        // Set view matrix.
        inline void SetViewMatrix(const Matrix44& viewMatrix)
        {
            m_ViewMatrix = viewMatrix;
        }

        // Get projection matrix.
        inline const Matrix44& GetProjectionMatrix() const
        {
            return m_ProjectionMatrix;
        }

        // Set projection matrix.
        inline void SetProjectionMatrix(const Matrix44& projectionMatrix)
        {
            m_ProjectionMatrix = projectionMatrix;
        }

        // Get current fov.
        inline float GetFOV() const
        {
            return m_FovAngle;
        }

        // Get current aspect.
        inline float GetAspect() const
        {
            return m_Aspect;
        }

        // Get near clipping planes.
        inline float GetNearClippingPlane() const
        {
            return m_NearClippingPlane;
        }

        // Get far clipping planes.
        inline float GetFarClippingPlane() const
        {
            return m_FarClippingPlane;
        }

        // Get current position.
        inline const Vector3& GetPosition() const
        {
            return m_Position;
        }

        // Set current position.
        inline void SetPosition(const Vector3& position)
        {
            m_Position = position;
        }

        // Set relative position.
        inline void SetRelativePosition(const Vector3& relativePosition)
        {
            m_Position += relativePosition;
        }

        // Get current rotation.
        inline const Quaternion& GetRotation() const
        {
            return m_Rotation;
        }

        // Set current rotation.
        inline void SetRotation(const Vector3& rotation)
        {
            Matrix44 matTemp;
            Matrix44RotationYawPitchRoll(matTemp, rotation);
            QuaternionFromMatrix44(m_Rotation, matTemp);
        }

        // Set current rotation.
        inline void SetRotation(const Quaternion& rotation)
        {
            m_Rotation = rotation;
        }

        // Set relative rotation.
        inline void SetRelativeRotation(const Vector3& relativeRotation)
        {
            Matrix44 matTemp;
            Matrix44RotationYawPitchRoll(matTemp, relativeRotation);

            Quaternion quatTemp;
            QuaternionFromMatrix44(quatTemp, matTemp);

            m_Rotation *= quatTemp;
        }

        // Set look at orientation.
        inline void SetLookAt(const Vector3& position, const Vector3& up)
        {
            Matrix44 matTemp;
            Matrix44LookAtLH(matTemp, m_Position, position, up);
            QuaternionFromMatrix44(m_Rotation, matTemp);
        }

        // Get forward vector.
        inline const Vector3& GetForward() const
        {
            return m_Forward;
        }

        // Get right vector.
        inline const Vector3& GetRight() const
        {
            return m_Right;
        }

        // Get up vector.
        inline const Vector3& GetUp() const
        {
            return m_Up;
        }

    private:
        // Build frustum after calculating projection and view.
        void BuildFrustum();

    private:
        // The graphics class as parent.
        class Graphics* m_Graphics;

        // The current used render target.
        Yw3dRenderTarget* m_RenderTarget;

        // If locked surfaces viewport or not.
        bool m_LockedSurfacesViewport;

        // World matrix.
        Matrix44 m_WorldMatrix;

        // View matrix.
        Matrix44 m_ViewMatrix;

        // Projection matrix.
        Matrix44 m_ProjectionMatrix;

        // Frustum plane.
        Plane m_Frustum[6];

        // FOV angle.
        float m_FovAngle;

        // Aspect ratio.
        float m_Aspect;

        // Near clipping plane.
        float m_NearClippingPlane;

        // Far clipping plane.
        float m_FarClippingPlane;

        // The camera position.
        Vector3 m_Position;

        // The camera rotation;
        Quaternion m_Rotation;

        // The forward vector.
        Vector3 m_Forward;

        // The right vector.
        Vector3 m_Right;

        // The up vector.
        Vector3 m_Up;
    };
}

#endif // !__YW_CAMERA_H__
