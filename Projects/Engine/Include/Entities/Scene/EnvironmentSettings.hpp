#pragma once
#include "Resource.h"

namespace Odyssey
{
	class EnvironmentSettings
	{
	public:
		GUID Skybox;
		glm::vec3 AmbientColor = glm::vec3(0.2f);
	};
}