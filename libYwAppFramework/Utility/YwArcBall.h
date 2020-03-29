// Add by Yaukey at 2020-03-29.
// YW Soft Renderer Arc-Ball class.
// Reference: CD3DArcBall in D3D-SDK sample project. File: DXUTcamera.h/cpp

#ifndef __YW_ARC_BALL_H__
#define __YW_ARC_BALL_H__

#include "YwBase.h"
#include "YwMath.h"

namespace yw
{
    class ArcBall
    {
    public:
        ArcBall();
        ~ArcBall();

    public:
        // Reset all parameters.
        void Reset();

        // Set window size and radius of arc ball.
        void SetWindow(int32_t width, int32_t height, float radius = 1.0f);

        // Set arc ball's offset.
        void SetOffset(int32_t nX, int32_t nY);

        // Set  Arc ball's radius for translating the target.
        void SetTranslationRadius(float radiusTranslation);

        // Call these from client and use GetRotationMatrix() to read new rotation matrix

        // Start the rotation (pass current mouse position).
        void OnBegin(int32_t x, int32_t y);

        // Continue the rotation (pass current mouse position).
        void OnMove(int32_t x, int32_t y);

        // End the rotation.
        void OnEnd();

        // Get final rotation matrix.
        const Matrix44& GetRotationMatrix();

        // Get final rotation quaternion.
        const Quaternion& GetRotation();

        // Get delta rotation since last move.
        const Quaternion& GetDeltaRotation();

        // Get if arc ball is being dragged or not.
        bool IsBeingDragged() const;

    private:
        // Convert scree point to normalized sphere point.
        Vector3	ScreenToVector(int32_t screenX, int32_t screenY);

        // Calculating the rotation quaternion from start point to end point on arch ball.
        Quaternion QuaternionFromBallPoints(Vector3& start, Vector3& end);

    private:
        // Window offset, or upper-left corner of window.
        Vector2 m_Offset;

        // Arc ball's window width.
        int32_t m_Width;

        // Arc ball's window height.
        int32_t m_Height;

        // Center of arc ball.
        Vector2 m_Center;

        // Arc ball's radius in screen coordinates.
        float m_Radius;

        // Arc ball's radius for translating the target.
        float m_RadiusTranslation;

        // Arc-Ball is dragged or not.
        bool m_IsDrag;

        // Previous rotation of this Arc-Ball.
        Quaternion m_PreviousRotation;

        // Current rotation.
        Quaternion m_CurrentRotation;

        // Delta rotation.
        Quaternion m_DeltaRotation;	

        // Previous moved point.
        Vector3 m_PreviousPoint;
        
        // Current point at moving stage.
        Vector3 m_CurrentPoint;

        // Previous move point.
        Vector3 m_PreviousMovePoint;

        // Final rotation matrix.
        Matrix44 m_RotateMatrix;
    };
}

#endif // __YW_ARC_BALL_H__
