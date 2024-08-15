#pragma once
#include "Assembly.hpp"
#include "Core.hpp"
#include "GameObject.h"

namespace Odyssey
{
	class ScriptBindings
	{
	public:
		static void Initialize(Coral::ManagedAssembly frameworkAssembly);
	};
}