#pragma once
#include "GameObject.h"

namespace Odyssey
{
	enum class LightType
	{
		None = 0,
		Directional = 1,
		Point = 2,
		Spot = 3,
		Area = 4,
	};

	class Light
	{
		CLASS_DECLARATION(Odyssey, Light)
	public:
		Light() = default;
		Light(const GameObject& gameObject);

	public:
		void Serialize(SerializationNode& node);
		void Deserialize(SerializationNode& node);

	public:
		void SetType(LightType lightType) { m_Type = lightType; }
		void SetColor(glm::vec3 color) { m_Color = color; }
		void SetIntensity(float intensity) { m_Intensity = intensity; }
		void SetRange(float range) { m_Range = range; }

	public:
		LightType GetType() { return m_Type; }
		glm::vec3 GetColor() { return m_Color; }
		float GetIntensity() { return m_Intensity; }
		float GetRange() { return m_Range; }

	public:
		glm::vec3 GetPosition();
		glm::vec3 GetDirection();

	private:
		LightType m_Type = LightType::Directional;
		glm::vec3 m_Color = glm::vec3(1.0f);
		float m_Intensity = 1.0f;
		float m_Range = 1.0f;

	private:
		GameObject m_GameObject;
	};
}