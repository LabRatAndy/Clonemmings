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
		void SetOrthographic(float size, float nearclip, float farclip, bool useaspectratio = true);
		void SetViewportSize(uint32_t width, uint32_t height, bool useaspectratio = true);
		float GetPerspectiveVerticalFOV() const { return m_PerspectiveFOV; }
		void SetPerspectiveVerticalFOV(float verticalFOV) { m_PerspectiveFOV = verticalFOV; RecalculateProjection(); }
		float GetPerspectiveNearClip() const { return m_PerspectiveNear; }
		void SetPerspectiveNearClip(float nearclip) { m_PerspectiveNear = nearclip; RecalculateProjection(); }
		float GetPerspectiveFarClip() const { return m_PerspectiveFar; }
		void SetPerspectiveFarClip(float farclip) { m_PerspectiveFar = farclip; RecalculateProjection(); }
		float GetOrthographicSize() const { return m_OrthographicSize; }
		void SetOrthographicSize(float size) { m_OrthographicSize = size; RecalculateProjection(false); }
		float GetOthographicNearClip() const { return m_OrthographicNear; }
		void SetOrthographicNearClip(float nearclip) { m_OrthographicNear = nearclip; RecalculateProjection(false); }
		float GetOrthographicFarClip() const { return m_OrthographicFar; }
		void SetOrthographicFarClip(float farclip) { m_OrthographicFar = farclip; RecalculateProjection(false); }
		ProjectionType GetProjectionType() const { return m_ProjectionType; }
		void SetProjectionType(ProjectionType projection) { m_ProjectionType = projection; RecalculateProjection(false); }
		float GetLevelHeight() const { return m_LevelHeight; }
		void SetLevelHeight(float levelheight) { m_LevelHeight = levelheight; RecalculateProjection(false); }
		float GetLevelWidth() const { return m_LevelWidth; }
		void SetLevelWidth(float levelwidth) { m_LevelWidth = levelwidth; RecalculateProjection(false); }
	private:
		ProjectionType m_ProjectionType = ProjectionType::Orthographic;
		float m_PerspectiveFOV = glm::radians(45.0f);
		float m_PerspectiveNear = 0.01f;
		float m_PerspectiveFar = 1000.0f;
		float m_OrthographicSize = 10.0f;
		float m_OrthographicNear = -1.0f;
		float m_OrthographicFar = 1.0;
		float m_AspectRatio = 0.0f;
		float m_LevelHeight = 50.f;
		float m_LevelWidth = 50.0f;
	
		void RecalculateProjection(bool useaspectratio = true);
	};
}