// Add by Yaukey at 2018-01-28.
// Camera class.

#include "stdafx.h"
#include "YwCamera.h"

namespace yw
{
	//////////////////////////////////////////////////////////////////////////
    Camera::Camera()
	{
		m_CamType = CT_AIRCRAFT;
		m_CamPerType = CPT_FIRST;

		m_Right = Vector3(1.0f, 0.0f, 0.0f);
		m_Up = Vector3(0.0f, 1.0f, 0.0f);
		m_Look = Vector3(0.0f, 0.0f, 1.0f);
		m_Position = Vector3(0.0f, 0.0f, 0.0f);
		m_RolePosition = Vector3(0.0f, -5.0f, -5.0f);

		Matrix44Identity(m_ViewMatrix);
        m_ViewChanged = true;
	}

	Camera::Camera(CAMERA_TYPE eType, CAMERA_PER_TYPE ePerType)
	{
		m_CamType = eType;
		m_CamPerType = ePerType;

		m_Right = Vector3(1.0f, 0.0f, 0.0f);
		m_Up = Vector3(0.0f, 1.0f, 0.0f);
		m_Look = Vector3(0.0f, 0.0f, 1.0f);
		m_Position = Vector3(0.0f, 0.0f, 0.0f);
		m_RolePosition = Vector3(0.0f, -5.0f, -5.0f);

        Matrix44Identity(m_ViewMatrix);
        m_ViewChanged = true;
	}

	Camera::~Camera()
	{
	}

	void Camera::Strafe(float unit)
	{
		Vector3 vUnit(0.0f, 0.0f, 0.0f);

		// Move only on xz plane for land obj.
		if (CT_LANDOBJECT == m_CamType)
		{
			vUnit = Vector3(m_Right.x, 0.0f, m_Right.z) * unit;
		}
		else if (CT_AIRCRAFT == m_CamType)
		{
			vUnit = m_Right * unit;
		}

		m_Position += vUnit;
		m_RolePosition += vUnit;
        m_ViewChanged = true;
	}

	void Camera::Fly(float unit)
	{
		// Move only on y-axis for land obj.
		if (CT_LANDOBJECT == m_CamType)
		{
			m_Position.y += unit;
			m_RolePosition.y += unit;
		}
		else if (CT_AIRCRAFT == m_CamType)
		{
			Vector3 vUnit = m_Up * unit;
			m_Position += vUnit;
			m_RolePosition += vUnit;
		}

		m_ViewChanged = true;
	}

	void Camera::Walk(float unit)
	{
		Vector3 vUnit(0.0f, 0.0f, 0.0f);

		// Move only on xz plane for land obj.
		if (CT_LANDOBJECT == m_CamType)
		{
			vUnit = Vector3(m_Look.x, 0.0f, m_Look.z) * unit;
		}
		else if (CT_AIRCRAFT == m_CamType)
		{
			vUnit = m_Look * unit;
		}

		m_Position += vUnit;
		m_RolePosition += vUnit;
		m_ViewChanged = true;
	}

	void Camera::Pitch(float angle)
	{
		Matrix44 mat;
		if (CPT_FIRST == m_CamPerType)
		{
            Matrix44RotationAxis(mat, m_Right, angle);

			// Rotate up and look vector around right.
			Vector3TransformCoord(m_Up, m_Up, mat);
            Vector3TransformCoord(m_Look, m_Look, mat);
		}
		else if (CPT_THIRD == m_CamPerType)
		{
            Matrix44RotationAxis(mat, m_Right, angle);

            Vector3TransformCoord(m_Look, m_Look, mat);
            Vector3TransformCoord(m_Up, m_Up, mat);
            Vector3TransformCoord(m_Position, m_Position - m_RolePosition, mat);

			m_Position += m_RolePosition;
		}

		m_ViewChanged = true;
	}

	void Camera::Yaw(float angle)
	{
        Matrix44 mat;
		if (CPT_FIRST == m_CamPerType)
		{
			if (CT_LANDOBJECT == m_CamType)
			{
                Matrix44RotationY(mat, angle);
			}
			else if (CT_AIRCRAFT == m_CamType)
			{
                Matrix44RotationAxis(mat, m_Up, angle);
			}

			// Rotate right and look vector around y.
            Vector3TransformCoord(m_Right, m_Right, mat);
            Vector3TransformCoord(m_Look, m_Look, mat);
		}
		else if (CPT_THIRD == m_CamPerType)
		{
            Matrix44RotationY(mat, angle);

            Vector3TransformCoord(m_Look, m_Look, mat);
            Vector3TransformCoord(m_Right, m_Right, mat);
            Vector3TransformCoord(m_Position, m_Position - m_RolePosition, mat);

			m_Position += m_RolePosition;
		}
		
		m_ViewChanged = true;
	}

	void Camera::Roll(float angle)
	{
		// Only roll for aircraft type.
		if (CT_AIRCRAFT == m_CamType)
		{
            Matrix44 mat;
            Matrix44RotationAxis(mat, m_Look, angle);

			// Rotate right and up vector around z.
            Vector3TransformCoord(m_Right, m_Right, mat);
            Vector3TransformCoord(m_Up, m_Up, mat);

			m_ViewChanged = true;
		}
	}

	void Camera::SetCameraType(CAMERA_TYPE type)
	{
		m_CamType = type;
	}

	int32_t Camera::GetCameraType() const
	{
		return (int32_t)m_CamType;
	}

	void Camera::SetCameraPerType(CAMERA_PER_TYPE perType)
	{
		m_CamPerType = perType;
	}

	int32_t Camera::GetCameraPerType() const
	{
		return (int32_t)m_CamPerType;
	}

	const Vector3* Camera::GetPosition(Vector3* position) const
	{
		if (nullptr != position)
		{
			*position = m_Position;
		}

		return &m_Position;
	}

	void Camera::SetPosition(const Vector3* position)
	{
		Vector3 vUnit = m_RolePosition - m_Position;
		m_Position = *position;
		m_RolePosition = m_Position + vUnit;
	}

	const Vector3* Camera::GetRolePosition(Vector3* position) const
	{
		if (nullptr != position)
		{
			*position = m_RolePosition;
		}

		return &m_RolePosition;
	}

	void Camera::SetRolePosition(const Vector3* position)
	{
		m_RolePosition = *position;
	}

	const Matrix44* Camera::GetViewMatrix(Matrix44* view)
	{
		if (!m_ViewChanged)
		{
			if (nullptr != view)
			{
				*view = m_ViewMatrix;
			}

			return &m_ViewMatrix;
		}

		// We use left-hand coordinate.
		// Keep the camera's axes orthogonal to eachother.
        Vector3Normalize(m_Look, m_Look);

		// Be careful of the cross order may result in different up vector orientation.
        Vector3Cross(m_Up, m_Look, m_Right);
        Vector3Normalize(m_Up, m_Up);

		// Be careful of the cross order may result in different right vector orientation.
        Vector3Cross(m_Right, m_Up, m_Look);
        Vector3Normalize(m_Right, m_Right);

		// Build the view matrix.
		float x = -Vector3Dot(m_Position, m_Right);
		float y = -Vector3Dot(m_Position, m_Up);
		float z = -Vector3Dot(m_Position, m_Look);

		m_ViewMatrix._11 = m_Right.x; m_ViewMatrix._12 = m_Up.x; m_ViewMatrix._13 = m_Look.x; m_ViewMatrix._14 = 0.0f;
		m_ViewMatrix._21 = m_Right.y; m_ViewMatrix._22 = m_Up.y; m_ViewMatrix._23 = m_Look.y; m_ViewMatrix._24 = 0.0f;
		m_ViewMatrix._31 = m_Right.z; m_ViewMatrix._32 = m_Up.z; m_ViewMatrix._33 = m_Look.z; m_ViewMatrix._34 = 0.0f;
		m_ViewMatrix._41 = x; m_ViewMatrix._42 = y; m_ViewMatrix._43 = z; m_ViewMatrix._44 = 1.0f;

		if (nullptr != view)
		{
			*view = m_ViewMatrix;
		}

		m_ViewChanged = false;
		return &m_ViewMatrix;
	}

	const Vector3* Camera::GetRightVector(Vector3* right) const
	{
		if (nullptr != right)
		{
			*right = m_Right;
		}
		
		return &m_Right;
	}

	const Vector3* Camera::GetUpVector(Vector3* up) const
	{
		if (nullptr != up)
		{
			*up = m_Up;
		}
		
		return &m_Up;
	}

	const Vector3* Camera::GetLookVector(Vector3* look) const
	{
		if (nullptr != look)
		{
			*look = m_Look;
		}
		
		return &m_Look;
	}
}
