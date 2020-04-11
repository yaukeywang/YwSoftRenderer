// Add by Yaukey at 2020-04-08.
// YW Soft Renderer arc ball camera class.

#include "YwArcBallCamera.h"
#include "YwArcBall.h"

namespace yw
{
    ArcBallCamera::ArcBallCamera(Graphics* graphics) :
        Camera(graphics),
        m_MinFromLookAtRadius(0.1f),
        m_MaxFromLookAtRadius(10.0f),
        m_FromLookAtRadius(1.5f),
        m_WorldArcBallEnabled(true),
        m_ViewArcBallEnabled(false),
        m_WorldArcBall(nullptr),
        m_ViewArcBall(nullptr)
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
        uint32_t screenWidth,
        uint32_t screenHeight,
        float fov,
        float aspect,
        float zNear,
        float zFar,
        const Vector3& position,
        const Vector3& lookAt,
        const Vector3& up,
        float minFromLookAtRadius,
        float maxFromLookAtRadius,
        bool worldArcBallEnabled,
        float worldArcBallRadius,
        int32_t worldArcBallRotationFlags,
        const Quaternion& worldArcBallInitRotation,
        bool viewArcBallEnabled,
        float viewArcBallRadius,
        int32_t viewArcBallRotationFlags,
        const Quaternion& viewArcBallInitRotation
    )
    {
        // Create camera.
        if (!CreateRenderCamera(screenWidth, screenHeight))
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
        m_MinFromLookAtRadius = minFromLookAtRadius;
        m_MaxFromLookAtRadius = maxFromLookAtRadius;
        m_FromLookAtRadius = (position - lookAt).Length();
        if (m_MaxFromLookAtRadius < m_MinFromLookAtRadius)
        {
            std::swap(m_MinFromLookAtRadius, m_MaxFromLookAtRadius);
        }

        // Initialize world and view arc ball.
        m_WorldArcBallEnabled = worldArcBallEnabled;
        m_ViewArcBallEnabled = viewArcBallEnabled;
        m_WorldArcBall->SetWindow((int32_t)screenWidth, (int32_t)screenHeight, worldArcBallRadius, worldArcBallRotationFlags, worldArcBallInitRotation);
        m_ViewArcBall->SetWindow((int32_t)screenWidth, (int32_t)screenHeight, viewArcBallRadius, viewArcBallRotationFlags, viewArcBallInitRotation);

        return true;
    }

    void ArcBallCamera::Reset()
    {
        m_WorldArcBall->Reset();
        m_ViewArcBall->Reset();
    }

    void ArcBallCamera::OnBegin(int32_t screenX, int32_t screenY)
    {
        if (m_WorldArcBallEnabled)
        {
            m_WorldArcBall->OnBegin(screenX, screenY);
        }

        if (m_ViewArcBallEnabled)
        {
            m_ViewArcBall->OnBegin(screenX, screenY);
            UpdateRotationByViewArchBall();
        }
    }

    void ArcBallCamera::OnMove(int32_t screenX, int32_t screenY)
    {
        if (m_WorldArcBallEnabled)
        {
            m_WorldArcBall->OnMove(screenX, screenY);
        }

        if (m_ViewArcBallEnabled)
        {
            m_ViewArcBall->OnMove(screenX, screenY);
            UpdateRotationByViewArchBall();
        }
    }

    void ArcBallCamera::OnEnd()
    {
        if (m_WorldArcBallEnabled)
        {
            m_WorldArcBall->OnEnd();
        }

        if (m_ViewArcBallEnabled)
        {
            m_ViewArcBall->OnEnd();
            UpdateRotationByViewArchBall();
        }
    }

    void ArcBallCamera::OnScroll(int32_t delta)
    {
        if (0 == delta)
        {
            return;
        }

        // Calculate new radius.
        m_FromLookAtRadius -= delta * (0.1f / 360.0f);
        m_FromLookAtRadius = max(m_MinFromLookAtRadius, m_FromLookAtRadius);
        m_FromLookAtRadius = min(m_MaxFromLookAtRadius, m_FromLookAtRadius);

        // Calculate new position.
        Vector3 newPosition = GetLookAtPosition() - GetForward() * m_FromLookAtRadius;

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
        SetPosition(lookAtPosition - worldForward * m_FromLookAtRadius);
        
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
