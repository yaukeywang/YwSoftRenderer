// Add by Yaukey at 2020-04-08.
// YW Soft Renderer arc ball camera class.

#ifndef __YW_ARC_BALL_CAMERA_H__
#define __YW_ARC_BALL_CAMERA_H__

#include "YwCamera.h"

namespace yw
{
    // Arc ball rotation axis.
    enum ArcBallCameraRotationAxis
    {
        ABCRA_All = 0,    // Rotate only around arbitrary axis.
        ABCRA_X = 1 << 0, // Rotate only around x axis.
        ABCRA_Y = 1 << 1, // Rotate only around y axis.
        ABCRA_Z = 1 << 2, // Rotate only around z axis.
        
    };

    // The base camera class.
    class ArcBallCamera : public Camera
    {
    public:
        // Constructor.
        ArcBallCamera(class Graphics* graphics);

        // Destructor.
        virtual ~ArcBallCamera();

    public:
        // Initialize camera with arc ball.
        // @param[in] screenWidth window width used by camera and arc ball.
        // @param[in] screenHeight window height used by camera and arc ball.
        // @param[in] fov fov for projection.
        // @param[in] aspect aspect for projection.
        // @param[in] zNear zNear for projection.
        // @param[in] zFar zFar for projection.
        // @param[in] position camera position.
        // @param[in] lookAt camera look at position.
        // @param[in] up camera up vector.
        // @param[in] minFromLookAtRadius camera min radius from look at position to camera position.
        // @param[in] maxFromLookAtRadius camera max radius from look at position to camera position.
        // @param[in] worldArcBallEnabled enable world arc ball or not.
        // @param[in] worldArcBallRadius world arc ball's radius.
        // @param[in] worldArcBallRotationFlags world arc ball rotation axis flags.
        // @param[in] viewArcBallEnabled enable view arc ball or not.
        // @param[in] viewArcBallRadius view arc ball's radius.
        // @param[in] viewArcBallRotationFlags view arc ball rotation axis flags.
        // @return bool true if the function succeeds, otherwise false.
        bool Initialize(
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
            bool worldArcBallEnabled = true,
            float worldArcBallRadius = 1.0f,
            int32_t worldArcBallRotationFlags = ABCRA_All,
            const Quaternion& worldArcBallInitRotation = Quaternion(0.0f, 0.0f, 0.0f, 1.0f),
            bool viewArcBallEnabled = false,
            float viewArcBallRadius = 1.0f,
            int32_t viewArcBallRotationFlags = ABCRA_All,
            const Quaternion& viewArcBallInitRotation = Quaternion(0.0f, 0.0f, 0.0f, 1.0f)
        );

        // Reset arc ball.
        void Reset();

        // On begin event.
        // @param[in] screenX x coordinate in client window.
        // @param[in] screenY x coordinate in client window.
        void OnBegin(int32_t screenX, int32_t screenY);

        // On move event.
        // @param[in] screenX x coordinate in client window.
        // @param[in] screenY x coordinate in client window.
        void OnMove(int32_t screenX, int32_t screenY);

        // On end event for view arc ball.
        void OnEnd();

        // On scroll event for camera.
        // @param[in] delta delta value of the scroll.
        void OnScroll(int32_t delta);

        // Get rotation from world arc ball.
        // @return final rotation quaternion from world arc ball.
        const Quaternion& GetWorldRotation() const;

        // Get rotation from world arc ball.
        // @return final rotation matrix from world arc ball.
        const Matrix44& GetWorldRotationMatrix();

        // Get rotation from view arc ball.
        // @return final rotation quaternion from view arc ball.
        const Quaternion& GetViewRotation() const;

        // Get rotation from view arc ball.
        // @return final rotation matrix from view arc ball.
        const Matrix44& GetViewRotationMatrix();

    protected:
        // Update camera rotation by view arch ball.
        void UpdateRotationByViewArchBall();

    protected:
        // Minimal radius from look at position to camera position.
        float m_MinFromLookAtRadius;

        // Maximum  radius from look at position to camera position.
        float m_MaxFromLookAtRadius;

        // View radius from look at position to camera position.
        float m_FromLookAtRadius;

        // Enable to use world arc ball or not.
        bool m_WorldArcBallEnabled;

        // Enable to use view arc ball or not.
        bool m_ViewArcBallEnabled;

        // The arc ball used for rotating object in world.
        class ArcBall* m_WorldArcBall;

        // The arc ball used for rotating this viewing camera.
        class ArcBall* m_ViewArcBall;  
    };
}

#endif // !__YW_ARC_BALL_CAMERA_H__
