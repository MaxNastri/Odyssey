#include "ParticleSystem.h"

namespace Odyssey
{
	ParticleSystem::ParticleSystem(const GameObject& gameObject)
	{

	}

	void ParticleSystem::Serialize(SerializationNode& node)
	{
		SerializationNode componentNode = node.AppendChild();
		componentNode.SetMap();
		componentNode.WriteData("Type", ParticleSystem::Type);
		componentNode.WriteData("Duration", Duration);
	}

	void ParticleSystem::Deserialize(SerializationNode& node)
	{
		node.ReadData("Duration", Duration);
	}
}