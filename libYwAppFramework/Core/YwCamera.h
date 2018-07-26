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

        // View distance.
        float m_ViewDistance;

        // The camera position.
        Vector3 m_Position;

        // The camera rotation;
        Quaternion m_Rotation;

        // The up vector.
        Vector3 m_Up;

        // The right vector.
        Vector3 m_Right;

        // The forward vector.
        Vector3 m_Forward;
    };
}

#endif // !__YW_CAMERA_H__
