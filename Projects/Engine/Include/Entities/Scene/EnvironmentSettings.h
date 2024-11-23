#pragma once
#include "Ref.h"
#include "Cubemap.h"

namespace Odyssey
{
	class EnvironmentSettings
	{
	public:
		void SetSkybox(GUID guid);

	public:
		Ref<Cubemap> Skybox;
		float3 AmbientColor = float3(0.2f);
	};
}