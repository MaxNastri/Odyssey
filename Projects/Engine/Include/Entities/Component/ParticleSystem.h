#pragma once
#include "GameObject.h"
#include "VulkanGlobals.h"

namespace Odyssey
{
	struct alignas(16) Particle
	{
		glm::vec4 Position = glm::vec4(0.0f);
		glm::vec4 Color = glm::vec4(1.0f);
		glm::vec4 Velocity = glm::vec4(0.0f, 0.1f, 0.0f, 0.0f);
		float Lifetime = 0.0f;
		float Size = 1.0f;
		float SizeOverLifetime = 0.0f;

		static VkVertexInputBindingDescription GetBindingDescription()
		{
			VkVertexInputBindingDescription bindingDescription;
			bindingDescription.binding = 0;
			bindingDescription.stride = sizeof(Particle);
			bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
			return bindingDescription;
		}

		static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions()
		{
			std::vector<VkVertexInputAttributeDescription> attributeDescriptions;

			VkVertexInputAttributeDescription description{};

			// Position
			description.binding = 0;
			description.location = 0;
			description.format = VK_FORMAT_R32G32B32_SFLOAT;
			description.offset = offsetof(Particle, Position);
			attributeDescriptions.push_back(description);

			description.binding = 0;
			description.location = 1;
			description.format = VK_FORMAT_R32G32B32_SFLOAT;
			description.offset = offsetof(Particle, Color);
			attributeDescriptions.push_back(description);

			return attributeDescriptions;
		}
	};

	struct ParticleEmitterData
	{
		float4 Position;
		float4 Color;
		float4 Velocity;
		float Lifetime;
		float Size;
		float Speed;
		uint EmitCount;
		uint EmitterIndex;
	};

	class ParticleSystem
	{
		CLASS_DECLARATION(Odyssey, ParticleSystem)
	public:
		ParticleSystem() = default;
		ParticleSystem(const GameObject& gameObject);

	public:
		void Serialize(SerializationNode& node);
		void Deserialize(SerializationNode& node);

	public:
		ParticleEmitterData& GetEmitterData() { return emitterData; }

	public:
		ParticleEmitterData emitterData;
		float Duration = 0.0f;
		// Particles spawned per sec
		uint32_t EmissionRate = 0;
		bool Loop = false;

	private:
		uint32_t m_ParticleCount;
		bool m_Playing;
	};
}