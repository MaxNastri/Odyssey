#pragma once
#include "Scene.h"

namespace Odyssey::Entities
{
	class ECS
	{
	public:
		static void Create();
		static void Load();
		static void Update();

	private:
		static Scene scene;

	};
}