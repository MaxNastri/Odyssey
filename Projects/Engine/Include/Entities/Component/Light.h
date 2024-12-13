#pragma once
#include "GameObject.h"

namespace Odyssey
{
	enum class LightType
	{
		Directional = 0,
		Point = 1,
		Spot = 2,
		Area = 3,
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
		void SetEnabled(bool enabled) { m_Enabled = enabled; }
		void SetType(LightType lightType) { m_Type = lightType; }
		void SetColor(glm::vec3 color) { m_Color = color; }
		void SetIntensity(float intensity) { m_Intensity = intensity; }
		void SetRange(float range) { m_Range = range; }

	public:
		bool IsEnabled() { return m_Enabled; }
		LightType GetType() { return m_Type; }
		glm::vec3 GetColor() { return m_Color; }
		float GetIntensity() { return m_Intensity; }
		float GetRange() { return m_Range; }

	public:
		glm::vec3 GetPosition();
		glm::vec3 GetDirection();
		static mat4 CalculateViewProj(float3 sceneCenter, float sceneRadius, float3 lightDir);

	private:
		LightType m_Type = LightType::Directional;
		glm::vec3 m_Color = glm::vec3(1.0f);
		float m_Intensity = 1.0f;
		float m_Range = 1.0f;
		float m_FOV = 45.0f;

	private:
		bool m_Enabled = true;
		GameObject m_GameObject;
	};
}