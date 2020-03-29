// Add by Yaukey at 2020-03-29.
// YW Soft Renderer Arc-Ball class.
// Reference: CD3DArcBall in D3D-SDK sample project. File: DXUTcamera.h/cpp

#include "YwArcBall.h"

namespace yw
{
    ArcBall::ArcBall()
    {
        Reset();

        m_Offset = Vector2::Zero();
        m_PreviousPoint = Vector2::Zero();
        m_CurrentPoint = Vector2::Zero();
        m_PreviousMovePoint = Vector2::Zero();

        SetWindow(m_Width, m_Height, m_Radius);
    }

    ArcBall::~ArcBall()
    {
    }

    void ArcBall::Reset()
    {
        QuaternionIdentity(m_PreviousRotation);
        QuaternionIdentity(m_CurrentRotation);
        QuaternionIdentity(m_DeltaRotation);
        Matrix44Identity(m_RotateMatrix);

        m_Width = 0;
        m_Height = 0;
        m_Radius = 1.0f;
        m_RadiusTranslation = 1.0f;
        m_IsDrag = false;
        
    }

    void ArcBall::SetWindow(int32_t width, int32_t height, float radius)
    {
        m_Width = width;
        m_Height = height;
        m_Radius = radius;
        m_Center = Vector2(m_Width / 2.0f, m_Height / 2.0f);
    }

    void ArcBall::SetOffset(int32_t x, int32_t y)
    {
        m_Offset.x = (float)x;
        m_Offset.y = (float)y;
    }

    void ArcBall::SetTranslationRadius(float radiusTranslation)
    {
        m_RadiusTranslation = radiusTranslation;
    }

    void ArcBall::OnBegin(int32_t x, int32_t y)
    {
        // Only enter the drag state if the click falls
        // inside the click rectangle.
        if (x >= (int32_t)m_Offset.x && x < (int32_t)m_Offset.x + m_Width && y >= (int32_t)m_Offset.y && y < (int32_t)m_Offset.y + m_Height)
        {
            // Begin drag state.
            m_IsDrag = true;
            m_PreviousRotation = m_CurrentRotation;
            m_PreviousPoint = ScreenToVector(x, y);
            m_PreviousMovePoint = m_PreviousPoint;
        }
    }

    void ArcBall::OnMove(int32_t x, int32_t y)
    {
        if (m_IsDrag)
        {
            m_CurrentPoint = ScreenToVector(x, y);
            m_DeltaRotation = QuaternionFromBallPoints(m_PreviousMovePoint, m_CurrentPoint);
            //m_CurrentRotation = m_PreviousRotation * QuaternionFromBallPoints(m_PreviousPoint, m_CurrentPoint);
            m_CurrentRotation = QuaternionFromBallPoints(m_PreviousPoint, m_CurrentPoint) * m_PreviousRotation;
            m_PreviousMovePoint = m_CurrentPoint;
        }
    }

    void ArcBall::OnEnd()
    {
        m_IsDrag = false;
    }

    const Matrix44& ArcBall::GetRotationMatrix()
    {
        return Matrix44FromQuaternion(m_RotateMatrix, m_CurrentRotation);
    }

    const Quaternion& ArcBall::GetRotation()
    {
        return m_CurrentRotation;
    }

    const Quaternion& ArcBall::GetDeltaRotation()
    {
        return m_DeltaRotation;
    }

    bool ArcBall::IsBeingDragged() const
    {
        return m_IsDrag;
    }

    Vector3 ArcBall::ScreenToVector(int32_t screenX, int32_t screenY)
    {
        // Scale to screen
        float x = -(screenX - m_Offset.x - m_Width / 2) / (m_Radius * m_Width / 2);
        float y = (screenY - m_Offset.y - m_Height / 2) / (m_Radius * m_Height / 2);

        float z = 0.0f;
        float mag = x * x + y * y;

        if (mag > 1.0f)
        {
            float scale = 1.0f / sqrtf(mag);
            x *= scale;
            y *= scale;
        }
        else
        {
            z = sqrtf(1.0f - mag);
        }

        return Vector3(x, y, z);
    }

    Quaternion ArcBall::QuaternionFromBallPoints(Vector3& start, Vector3& end)
    {
        // Calculate rotate angle.
        float angle = Vector3Dot(start, end);

        // Calculate rotate axis
        Vector3 axis;
        Vector3Cross(axis, start, end);

        // Build and Normalize the Quaternion.
        Quaternion quat(axis.x, axis.y, axis.z, angle);
        QuaternionNormalize(quat, quat);

        return quat;
    }
}
