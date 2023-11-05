#pragma once
#include "Component.h"
#include <Vector4.h>

namespace Odyssey::Entities
{
	class Transform : public Component
	{
	public:
		Transform() = default;
		Vector4 position;
	};
}