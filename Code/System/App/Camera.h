// Add by Yaukey at 2018-01-28.
// Camera class.
// Need improve: frustum, apsect ratio, cull, viewport, perspective...

#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "System/Support/Base.h"
#include "Driven/Math/MathCommon.h"

namespace yw
{
	//////////////////////////////////////////////////////////////////////////
	// The camera.
	class Camera
	{
	public:
		enum CAMERA_TYPE
		{
			CT_INVALID = -1,
			CT_LANDOBJECT,
			CT_AIRCRAFT,
			CT_END
		};

		enum CAMERA_PER_TYPE
		{
			CPT_INVALID = -1,
			CPT_FIRST,
			CPT_THIRD,
			CPT_END
		};

	public:
        Camera();
        Camera(CAMERA_TYPE type, CAMERA_PER_TYPE perType);
		virtual ~Camera();

	public:
		//////////////////////////////////////////////////////////////////////////

		// Go left or right.
		void Strafe(float unit);

		// Go up or down.
		void Fly(float unit);

		// Go forward or backward.
		void Walk(float unit);

		//////////////////////////////////////////////////////////////////////////

		// Rotate on right vector.
		void Pitch(float angle);

		// Rotate on up vector.
		void Yaw(float angle);

		// Rotate on look vector.
		void Roll(float angle);

		//////////////////////////////////////////////////////////////////////////

		// Set camera type.
		void SetCameraType(CAMERA_TYPE type);

		// Get camera type.
		int32_t GetCameraType() const;

		// Set camera perspective type.
		void SetCameraPerType(CAMERA_PER_TYPE perType);

		// Get camera perspective type.
		int32_t GetCameraPerType() const;

		// Get camera position.
		const Vector3* GetPosition(Vector3* position = nullptr) const;

		// Set camera new position.
		void SetPosition(const Vector3* position);

		// Get the virtual role position of this camera.
		const Vector3* GetRolePosition(Vector3* position = nullptr) const;

		// Set the virtual role position of this camera.
		void SetRolePosition(const Vector3* position);

		//////////////////////////////////////////////////////////////////////////

		// Calculate view matrix.
		const Matrix44* GetViewMatrix(Matrix44* view = nullptr);

		// Get right vector.
		const Vector3* GetRightVector(Vector3* right = nullptr) const;

		// Get up vector.
		const Vector3* GetUpVector(Vector3* up = nullptr) const;

		// Get look vector.
		const Vector3* GetLookVector(Vector3* look = nullptr) const;

	protected:
		CAMERA_TYPE m_CamType;         // The type of camera.
		CAMERA_PER_TYPE m_CamPerType;	// The perspective mode of this camera.

		Vector3 m_Right;                // The right vector(x axis).
		Vector3 m_Up;                   // The up vector(y axis).
		Vector3 m_Look;                 // The look vector(z axis).
		Vector3 m_Position;             // The current position of camera.
		Vector3 m_RolePosition;         // The role position for the camera when using 3rd person perspective.
		Matrix44 m_ViewMatrix;          // The current view matrix.

		bool m_ViewChanged;             // If the view matrix of this camera has been changed.
	};
}

#endif // __CAMERA_H__
