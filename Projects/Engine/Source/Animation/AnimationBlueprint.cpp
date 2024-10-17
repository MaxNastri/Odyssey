#include "AnimationBlueprint.h"

namespace Odyssey
{
	bool AnimationBlueprint::SetProperty(const std::string& name, bool value)
	{
		if (!m_Properties.contains(name))
			m_Properties[name].ValueBuffer.Allocate(sizeof(value));

		RawBuffer& valueBuffer = m_Properties[name].ValueBuffer;
		valueBuffer.Write(&value, sizeof(value));

		return true;
	}
}