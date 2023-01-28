#include "Core/Renderer/SceneCamera.h"
#include "Core/Application/Assert.h"
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace Clonemmings
{
	SceneCamera::SceneCamera()
	{
		RecalculateProjection();
	}
	void SceneCamera::SetPerspective(float verticalPOV, float nearclip, float farclip)
	{
		m_ProjectionType = ProjectionType::Perspective;
		m_PerspectiveFOV = verticalPOV;
		m_PerspectiveNear = nearclip;
		m_PerspectiveFar = farclip;
		RecalculateProjection();
	}
	void SceneCamera::SetOrthographic(float size, float nearclip, float farclip)
	{
		m_ProjectionType = ProjectionType::Orthographic;
		m_OrthographicSize = size;
		m_OrthographicNear = nearclip;
		m_OrthographicFar = farclip;
		RecalculateProjection();
	}
	void SceneCamera::SetViewportSize(uint32_t width, uint32_t height)
	{
		ASSERT(width > 0 && height > 0, "Width and height must be greater than 0!");
		m_AspectRatio = (float)width / (float)height;
		RecalculateProjection();
	}
	void SceneCamera::RecalculateProjection()
	{
		if (m_ProjectionType == ProjectionType::Perspective)
		{
			m_Projection = glm::perspective(m_PerspectiveFOV, m_AspectRatio, m_PerspectiveNear, m_PerspectiveFar);
		}
		else
		{
			float ortholeft = -m_OrthographicSize * m_AspectRatio * 0.5f;
			float orthoright = m_OrthographicSize * m_AspectRatio * 0.5f;
			float orthotop = m_OrthographicSize * 0.5f;
			float orthobottom = -m_OrthographicSize * 0.5f;
			m_Projection = glm::ortho(ortholeft, orthoright, orthobottom, orthotop, m_OrthographicNear, m_OrthographicFar);
		}
	}
}