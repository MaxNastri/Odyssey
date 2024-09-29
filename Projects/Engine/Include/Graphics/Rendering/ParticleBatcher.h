#pragma once
#include "ParticleSystem.h"

namespace Odyssey
{
	class ParticleBatcher
	{
	public:
		static void Init();
		static void Shutdown();

	public:
		static void Update(const std::vector<ParticleSystem>& systems);

	private:
	};
}