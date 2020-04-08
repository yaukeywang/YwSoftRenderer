// Add by Yaukey at 2020-04-08.
// YW Soft Renderer arc ball camera class.

#include "YwArcBallCamera.h"
#include "YwArcBall.h"

namespace yw
{
    ArcBallCamera::ArcBallCamera(Graphics* graphics) :
        Camera(graphics),
        m_WorldArcBall(nullptr),
        m_ViewArcBall(nullptr),
        m_MinZoomDistance(0.1f),
        m_MaxZoomDistance(10.0f)
    {
        m_WorldArcBall = new ArcBall();
        m_ViewArcBall = new ArcBall();

        m_WorldArcBall->Reset();
        m_ViewArcBall->Reset();
    }

    ArcBallCamera::~ArcBallCamera()
    {
        YW_SAFE_DELETE(m_WorldArcBall);
        YW_SAFE_DELETE(m_ViewArcBall);
    }

    bool ArcBallCamera::Initialize(
        uint32_t width, 
        uint32_t height, 
        float worldArcBallRadius, 
        float viewArcBallRadius,
        float fov, 
        float aspect, 
        float zNear, 
        float zFar,
        const Vector3& position, 
        const Vector3& lookAt, 
        const Vector3& up, 
        float minZoom, 
        float maxZoom
    )
    {
        // Create camera.
        if (!CreateRenderCamera(width, height))
        {
            return false;
        }

        // Calculation projection matrix.
        CalculateProjection(fov, aspect, zNear, zFar);

        // Calculation view matrix.
        SetPosition(position);
        SetLookAt(lookAt, up);
        CalculateView();

        // Set zoom info.
        m_MinZoomDistance = minZoom;
        m_MaxZoomDistance = maxZoom;
        if (m_MaxZoomDistance < m_MinZoomDistance)
        {
            std::swap(m_MinZoomDistance, m_MaxZoomDistance);
        }

        // Init arc ball.
        m_WorldArcBall->SetWindow((int32_t)width, (int32_t)height, worldArcBallRadius);
        m_ViewArcBall->SetWindow((int32_t)width, (int32_t)height, viewArcBallRadius);

        return true;
    }

    void ArcBallCamera::ResetArcBall()
    {
        m_WorldArcBall->Reset();
        m_ViewArcBall->Reset();
    }

    void ArcBallCamera::OnBeginWorldArcBall(int32_t screenX, int32_t screenY)
    {
        m_WorldArcBall->OnBegin(screenX, screenY);
    }

    void ArcBallCamera::OnMoveWorldArcBall(int32_t screenX, int32_t screenY)
    {
        m_WorldArcBall->OnMove(screenX, screenY);
    }

    void ArcBallCamera::OnEndWorldArcBall()
    {
        m_WorldArcBall->OnEnd();
    }

    void ArcBallCamera::OnBeginViewArcBall(int32_t screenX, int32_t screenY)
    {
        m_ViewArcBall->OnBegin(screenX, screenY);
    }

    void ArcBallCamera::OnMoveViewArcBall(int32_t screenX, int32_t screenY)
    {
        m_ViewArcBall->OnMove(screenX, screenY);
    }

    void ArcBallCamera::OnEndViewArcBall()
    {
        m_ViewArcBall->OnEnd();
    }

    void ArcBallCamera::OnScroll(int32_t delta)
    {
        if (0 == delta)
        {
            return;
        }

        // Calculate new position.
        Vector3 newPosition = GetPosition() - m_Forward * -(delta * (0.1f / 360.0f));
        float distance = (newPosition - GetLookAtPosition()).SquaredLength();
        if (distance < (m_MinZoomDistance * m_MinZoomDistance) || distance >(m_MaxZoomDistance * m_MaxZoomDistance))
        {
            return;
        }

        // Set new position.
        SetPosition(newPosition);
        SetLookAt(m_LookAtPosition, m_Up);
        CalculateView();
    }

    const Quaternion& ArcBallCamera::GetWorldRotation() const
    {
        return m_WorldArcBall->GetRotation();
    }

    const Matrix44& ArcBallCamera::GetWorldRotationMatrix()
    {
        return m_WorldArcBall->GetRotationMatrix();
    }

    const Quaternion& ArcBallCamera::GetViewRotation() const
    {
        return m_ViewArcBall->GetRotation();
    }

    const Matrix44& ArcBallCamera::GetViewRotationMatrix()
    {
        return m_ViewArcBall->GetRotationMatrix();
    }
}
