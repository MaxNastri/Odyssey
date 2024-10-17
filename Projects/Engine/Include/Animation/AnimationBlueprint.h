#pragma once
#include "Blueprint.h"
#include "RawBuffer.hpp"

namespace Odyssey
{
	struct Property
	{
		std::string Name;
		RawBuffer ValueBuffer;
	};

	class AnimationBlueprint
	{
	public:
		bool SetProperty(const std::string& name, bool value);

	private:
		std::map<std::string, Property> m_Properties;
		Rune::Blueprint m_Blueprint;
	};
}