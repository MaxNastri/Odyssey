#pragma once
#include "RawBuffer.hpp"

namespace Odyssey
{
	enum class AnimationPropertyType
	{
		None = 0,
		Float = 1,
		Int = 2,
		Bool = 3,
		Trigger = 4,
	};

	struct AnimationProperty
	{
		std::string Name;
		AnimationPropertyType Type;
		RawBuffer ValueBuffer;

	public:
		AnimationProperty(std::string_view name, AnimationPropertyType type)
			: Name(name), Type(type)
		{
			switch (type)
			{
				case AnimationPropertyType::None:
					break;
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
	};
}