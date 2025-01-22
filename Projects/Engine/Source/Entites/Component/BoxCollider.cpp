#include "BoxCollider.h"
#include "PhysicsLayers.h"
#include "Enum.h"

namespace Odyssey
{
	BoxCollider::BoxCollider(const GameObject& gameObject)
		: m_GameObject(gameObject)
	{

	}

	BoxCollider::BoxCollider(const GameObject& gameObject, SerializationNode& node)
		: m_GameObject(gameObject)
	{
		Deserialize(node);
	}

	void BoxCollider::Serialize(SerializationNode& node)
	{
		SerializationNode componentNode = node.AppendChild();
		componentNode.SetMap();

		componentNode.WriteData("Type", BoxCollider::Type);
		componentNode.WriteData("Enabled", m_Enabled);
		componentNode.WriteData("Physics Layer", Enum::ToString(m_PhysicsLayer));
		componentNode.WriteData("Center", m_Center);
		componentNode.WriteData("Extents", m_Extents);
	}

	void BoxCollider::Deserialize(SerializationNode& node)
	{
		std::string layer;
		node.ReadData("Enabled", m_Enabled);
		node.ReadData("Physics Layer", layer);
		node.ReadData("Center", m_Center);
		node.ReadData("Extents", m_Extents);

		if (!layer.empty())
			m_PhysicsLayer = Enum::ToEnum<PhysicsLayer>(layer);
	}

	void BoxCollider::SetEnabled(bool enabled)
	{
		m_Enabled = enabled;
	}
}