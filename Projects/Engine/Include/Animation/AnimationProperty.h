#pragma once
#include "RawBuffer.h"

namespace Odyssey
{
	enum class AnimationPropertyType
	{
		Float = 0,
		Int = 1,
		Bool = 2,
		Trigger = 3,
	};

	struct AnimationProperty
	{
	public:
		AnimationProperty() = default;
		AnimationProperty(std::string_view name, AnimationPropertyType type)
			: Name(name), Type(type)
		{
			switch (type)
			{
				case AnimationPropertyType::Float:
					ValueBuffer.Allocate(sizeof(float));
					break;
				case AnimationPropertyType::Int:
					ValueBuffer.Allocate(sizeof(int32_t));
					break;
				case AnimationPropertyType::Bool:
					ValueBuffer.Allocate(sizeof(bool));
					break;
				case AnimationPropertyType::Trigger:
					ValueBuffer.Allocate(sizeof(bool));
					break;
				default:
					break;
			}
		}

	public:
		std::string Name;
		AnimationPropertyType Type;
		RawBuffer ValueBuffer;
	};
}