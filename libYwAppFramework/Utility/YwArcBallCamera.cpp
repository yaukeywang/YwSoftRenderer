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
        m_MinViewRadius(0.1f),
        m_MaxViewRadius(10.0f),
        m_ViewRadius(1.5f)
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
        float minViewRadius, 
        float maxViewRadius
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
        m_MinViewRadius = minViewRadius;
        m_MaxViewRadius = maxViewRadius;
        m_ViewRadius = (position - lookAt).Length();
        if (m_MaxViewRadius < m_MinViewRadius)
        {
            std::swap(m_MinViewRadius, m_MaxViewRadius);
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
        UpdateRotationByViewArchBall();
    }

    void ArcBallCamera::OnMoveViewArcBall(int32_t screenX, int32_t screenY)
    {
        m_ViewArcBall->OnMove(screenX, screenY);
        UpdateRotationByViewArchBall();
    }

    void ArcBallCamera::OnEndViewArcBall()
    {
        m_ViewArcBall->OnEnd();
        UpdateRotationByViewArchBall();
    }

    void ArcBallCamera::OnScroll(int32_t delta)
    {
        if (0 == delta)
        {
            return;
        }

        // Calculate new radius.
        m_ViewRadius -= delta * (0.1f / 360.0f);
        m_ViewRadius = max(m_MinViewRadius, m_ViewRadius);
        m_ViewRadius = min(m_MaxViewRadius, m_ViewRadius);

        // Calculate new position.
        Vector3 newPosition = GetLookAtPosition() - GetForward() * m_ViewRadius;

        // Set new position.
        SetPosition(newPosition);
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

    void ArcBallCamera::UpdateRotationByViewArchBall()
    {
        // Get view arc ball inverse rotation.
        Matrix44 viewArcBallRotation;
        Matrix44Inverse(viewArcBallRotation, m_ViewArcBall->GetRotationMatrix());

        // Calculating up vector by rotation.
        Vector3 worldUp;
        Vector3 localUp(0.0f, 1.0f, 0.0f);
        Vector3TransformCoord(worldUp, localUp, viewArcBallRotation);

        // Calculating forward vector by rotation.
        Vector3 worldForward;
        Vector3 localForward(0.0f, 0.0f, 1.0f);
        Vector3TransformCoord(worldForward, localForward, viewArcBallRotation);

        // Calculating new camera position from look at position.
        const Vector3& lookAtPosition = GetLookAtPosition();
        SetPosition(lookAtPosition - worldForward * m_ViewRadius);
        
        // Get final look at matrix.
        Matrix44 matTemp;
        Matrix44LookAtLH(matTemp, GetPosition(), lookAtPosition, worldUp);

        // Get final rotation that applied to view matrix.
        Quaternion finalRotation;
        QuaternionFromMatrix44(finalRotation, matTemp);
        SetRotation(finalRotation);

        // Update view matrix.
        CalculateView();
    }
}
