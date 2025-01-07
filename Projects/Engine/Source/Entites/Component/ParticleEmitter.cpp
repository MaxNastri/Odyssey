#include "ParticleEmitter.h"
#include "Transform.h"
#include "ParticleBatcher.h"

namespace Odyssey
{
	ParticleEmitter::ParticleEmitter(const GameObject& gameObject)
	{
		m_GameObject = gameObject;
		m_EmissionCount = m_EmissionRate;
	}

	void ParticleEmitter::Serialize(SerializationNode& node)
	{
		SerializationNode componentNode = node.AppendChild();
		componentNode.SetMap();
		componentNode.WriteData("Type", ParticleEmitter::Type);
		componentNode.WriteData("Enabled", m_Enabled);
		componentNode.WriteData("Duration", m_Duration);
		componentNode.WriteData("Radius", emitterData.Radius);
		componentNode.WriteData("Angle", emitterData.Angle);
		componentNode.WriteData("Looping", m_Looping);
		componentNode.WriteData("Material", m_Material.CRef());
		componentNode.WriteData("Emission Rate", m_EmissionRate);
		componentNode.WriteData("Start Color", emitterData.StartColor);
		componentNode.WriteData("End Color", emitterData.EndColor);
		componentNode.WriteData("Velocity", emitterData.Velocity);
		componentNode.WriteData("Lifetime", emitterData.Lifetime);
		componentNode.WriteData("Size", emitterData.Size);
		componentNode.WriteData("Speed", emitterData.Speed);
	}

	void ParticleEmitter::Deserialize(SerializationNode& node)
	{
		node.ReadData("Enabled", m_Enabled);
		node.ReadData("Duration", m_Duration);
		node.ReadData("Radius", emitterData.Radius);
		node.ReadData("Angle", emitterData.Angle);
		node.ReadData("Looping", m_Looping);
		node.ReadData("Emission Rate", m_EmissionRate);
		node.ReadData("Material", m_Material.Ref());
		node.ReadData("Start Color", emitterData.StartColor);
		node.ReadData("End Color", emitterData.EndColor);
		node.ReadData("Velocity", emitterData.Velocity);
		node.ReadData("Lifetime", emitterData.Lifetime);
		node.ReadData("Size", emitterData.Size);
		node.ReadData("Speed", emitterData.Speed);
	}

	ParticleEmitterData& ParticleEmitter::GetEmitterData()
	{
		if (Transform* transform = m_GameObject.TryGetComponent<Transform>())
		{
			glm::mat4 world = transform->GetWorldMatrix();
			emitterData.Position = float4(world[3][0], world[3][1], world[3][2], 1.0f);
			emitterData.Velocity = float4(glm::normalize(transform->Up()), 0.0f);
		}
		return emitterData;
	}

	void ParticleEmitter::Update(float deltaTime)
	{
		float emissionInterval = 1.0f / (float)m_EmissionRate;
		emitterData.EmitCount = 0;

		m_ActiveTime += deltaTime;

		if (m_ActiveTime >= m_Duration)
		{
			if (!m_Looping)
			{
				m_Active = false;
				return;
			}
			else
				m_Active = 0.0f;
		}

		// Update our time since last emission
		m_EmissionTime += deltaTime;

		if (m_EmissionTime > emissionInterval)
		{
			uint32_t toSpawn = (uint32_t)std::floor(m_EmissionTime / emissionInterval);
			m_EmissionTime = 0.0f;
			emitterData.EmitCount = toSpawn;
		}
	}
}