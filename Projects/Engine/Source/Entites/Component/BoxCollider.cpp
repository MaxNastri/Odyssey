#include "BoxCollider.h"
#include "PhysicsLayers.h"
#include "Enum.h"
#include "Transform.h"
#include "DebugRenderer.h"

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

	void BoxCollider::SetDebugEnabled(bool enabled)
	{
		if (enabled != m_DebugEnabled)
		{
			m_DebugEnabled = enabled;

			if (m_DebugEnabled)
				m_DebugID = DebugRenderer::Register([this]() { DebugDraw(); });
			else
				DebugRenderer::Deregister(m_DebugID);
		}
	}
	void BoxCollider::DebugDraw()
	{
		if (Transform* transform = m_GameObject.TryGetComponent<Transform>())
		{
			float3 position, scale;
			quat rotation;
			transform->DecomposeWorldMatrix(position, rotation, scale);

			float3 right = transform->Right();
			float3 up = transform->Up();
			float3 forward = transform->Forward();

			DebugRenderer::AddOrientedBox(position + m_Center, m_Extents, right, up, forward, float3(0.0f, 1.0f, 0.0f));
		}
	}
}