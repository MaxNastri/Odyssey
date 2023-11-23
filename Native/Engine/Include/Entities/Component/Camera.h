#pragma once
#include "Component.h"
#include "glm.h"

namespace Odyssey
{
	class Transform;

	class Camera : public Component
	{
	public:
		virtual void Awake() override;
		virtual void Serialize(ryml::NodeRef& node) override;
		virtual void Deserialize(ryml::ConstNodeRef node) override;

	public:
		glm::mat4 GetProjection() { return m_Projection; }
		glm::mat4 GetInverseView();

	private:
		void CalculateProjection();
		void CalculateInverseView();

	private: // Serialized
		CLASS_DECLARATION(Camera);
		float m_FieldOfView = 45.0f;
		float m_NearClip = 0.1f;
		float m_FarClip = 1000.0f;

	private: // Non-serialized
		Transform* m_Transform;
		glm::mat4 m_Projection;
		glm::mat4 m_InverseView;
		float m_Width;
		float m_Height;
	};
}