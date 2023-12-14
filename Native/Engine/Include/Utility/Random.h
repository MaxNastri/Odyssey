#pragma once
#include "glm.h"
#include <random>

namespace Odyssey
{
	class Random
	{
	public:
		static void Initialize()
		{
			s_RandomEngine.seed(std::random_device()());
		}

		static float Float()
		{
			return (float)s_Distribution(s_RandomEngine) / (float)std::numeric_limits<uint32_t>::max();
		}

		static glm::vec3 Vector3()
		{
			return glm::vec3(Float(), Float(), Float());
		}

		static glm::vec3 Vector3(float min, float max)
		{
			return glm::vec3(Float() * (max - min) + min, Float() * (max - min) + min, Float() * (max - min) + min);
		}

		static glm::vec3 InUnitSphere()
		{
			return glm::normalize(Vector3(-1.0f, 1.0f));
		}
	private:
		thread_local static std::mt19937 s_RandomEngine;
		thread_local static std::uniform_int_distribution<std::mt19937::result_type> s_Distribution;
	};
}