#pragma once
#include "Core/Scene/Scene.h"

namespace Clonemmings
{
	class SceneSerialiser
	{
	public:
		SceneSerialiser(const std::shared_ptr<Scene>& scene);

		void Serialise(const std::string& filename);
		bool Deserialise(const std::string& filename);

	private:
		std::shared_ptr<Scene> m_Scene = nullptr;

	};
}