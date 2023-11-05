#pragma once
#include "Scene.h"

namespace Odyssey::Entities
{
	class ECS
	{
	public:
		static void Create();
		static void Update();

	private:
		static Scene scene;

	};
}