#pragma once
#include "Resource.h"
#include "Ref.h"
#include "Cubemap.h"
#include "AssetManager.h"

namespace Odyssey
{
	class EnvironmentSettings
	{
	public:
		void SetSkybox(GUID guid) { Skybox = AssetManager::LoadAsset<Cubemap>(guid); }

	public:
		Ref<Cubemap> Skybox;
		glm::vec3 AmbientColor = glm::vec3(0.2f);
	};
}