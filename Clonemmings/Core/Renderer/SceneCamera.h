#pragma once

#include "Core/Application/Camera.h"

namespace Clonemmings
{
	class SceneCamera : public Camera
	{
	public:
		enum class ProjectionType
		{
			Perspective = 0,
			Orthographic = 1
		};

		SceneCamera();
		virtual ~SceneCamera() = default;
		void SetPerspective(float verticalPOV, float nearclip, float farclip);
		void SetOrthographic(float size, float nearclip, float farclip);
		void SetViewportSize(uint32_t width, uint32_t height);
		float GetPerspectiveVerticalFOV() const { return m_PerspectiveFOV; }
		void SetPerspectiveVerticalFOV(float verticalFOV) { m_PerspectiveFOV = verticalFOV; RecalculateProjection(); }
		float GetPerspectiveNearClip() const { return m_PerspectiveNear; }
		void GetPerspectiveNearClip(float nearclip) { m_PerspectiveNear = nearclip; RecalculateProjection(); }
		float GetPerspectiveFarClip() const { return m_PerspectiveFar; }
		void SetPerspectiveFarClip(float farclip) { m_PerspectiveFar = farclip; RecalculateProjection(); }
		float GetOrthographicSize() const { return m_OrthographicSize; }
		void SetOrthographicSize(float size) { m_OrthographicSize = size; RecalculateProjection(); }
		float GetOthographicNearClip() const { return m_OrthographicNear; }
		void SetOrthographicNearClip(float nearclip) { m_OrthographicNear = nearclip; RecalculateProjection(); }
		float GetOrthographicFarClip() const { return m_OrthographicFar; }
		void SetOrthographicFarClip(float farclip) { m_OrthographicFar = farclip; RecalculateProjection(); }
		ProjectionType GetProjectionType() const { return m_ProjectionType; }
		void SetProjectionType(ProjectionType projection) { m_ProjectionType = projection; RecalculateProjection(); }
	private:
		ProjectionType m_ProjectionType = ProjectionType::Orthographic;
		float m_PerspectiveFOV = glm::radians(45.0f);
		float m_PerspectiveNear = 0.01f;
		float m_PerspectiveFar = 1000.0f;
		float m_OrthographicSize = 10.0f;
		float m_OrthographicNear = -1.0f;
		float m_OrthographicFar = 1.0;
		float m_AspectRatio = 0.0f;
	
		void RecalculateProjection();
	};
}