// Add by Yaukey at 2020-04-08.
// YW Soft Renderer arc ball camera class.

#ifndef __YW_ARC_BALL_CAMERA_H__
#define __YW_ARC_BALL_CAMERA_H__

#include "YwCamera.h"

namespace yw
{
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
        // @param[in] width window width used by camera and arc ball.
        // @param[in] height window height used by camera and arc ball.
        // @param[in] worldArcBallRadius world arc ball's radius.
        // @param[in] viewArcBallRadius view arc ball's radius.
        // @param[in] fov fov for projection.
        // @param[in] aspect aspect for projection.
        // @param[in] zNear zNear for projection.
        // @param[in] zFar zFar for projection.
        // @param[in] position camera position.
        // @param[in] lookAt camera look at position.
        // @param[in] up camera up vector.
        // @param[in] minViewRadius camera min radius from look at position to camera position.
        // @param[in] maxViewRadius camera max radius from look at position to camera position.
        // @return bool true if the function succeeds, otherwise false.
        bool Initialize(
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
        );

        // Reset arc ball.
        void ResetArcBall();

        // On begin event for world arc ball.
        // @param[in] screenX x coordinate in client window.
        // @param[in] screenY x coordinate in client window.
        void OnBeginWorldArcBall(int32_t screenX, int32_t screenY);

        // On move event for world arc ball.
        // @param[in] screenX x coordinate in client window.
        // @param[in] screenY x coordinate in client window.
        void OnMoveWorldArcBall(int32_t screenX, int32_t screenY);

        // On end event for world arc ball.
        void OnEndWorldArcBall();

        // On begin event for view arc ball.
        // @param[in] screenX x coordinate in client window.
        // @param[in] screenY x coordinate in client window.
        void OnBeginViewArcBall(int32_t screenX, int32_t screenY);

        // On move event for view arc ball.
        // @param[in] screenX x coordinate in client window.
        // @param[in] screenY x coordinate in client window.
        void OnMoveViewArcBall(int32_t screenX, int32_t screenY);

        // On end event for view arc ball.
        void OnEndViewArcBall();

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

    private:
        // Update camera rotation by view arch ball.
        void UpdateRotationByViewArchBall();

    private:
        // The arc ball used for rotating object in world.
        class ArcBall* m_WorldArcBall;

        // The arc ball used for rotating this viewing camera.
        class ArcBall* m_ViewArcBall;

        // Minimal zoom.
        float m_MinViewRadius;

        // Maximum zoom.
        float m_MaxViewRadius;

        // View radius from look at position to camera position.
        float m_ViewRadius;
    };
}

#endif // !__YW_ARC_BALL_CAMERA_H__
