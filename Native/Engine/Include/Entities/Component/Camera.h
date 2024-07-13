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
		bool IsMainCamera() { return m_MainCamera; }
		glm::mat4 GetProjection() { return m_Projection; }
		glm::mat4 GetInverseProjection() { return m_InverseProjection; }
		glm::mat4 GetInverseView();
		glm::mat4 GetView();
		float GetFieldOfView() { return m_FieldOfView; }
		float GetNearClip() { return m_NearClip; }
		float GetFarClip() { return m_FarClip; }
		void SetFieldOfView(float fov);
		void SetNearClip(float nearClip);
		void SetFarClip(float farClip);
		void SetViewportSize(float width, float height);

	private:
		void CalculateProjection();
		void CalculateInverseView();

	private: // Serialized
		float m_FieldOfView = 45.0f;
		float m_NearClip = 0.1f;
		float m_FarClip = 1000.0f;
		bool m_MainCamera = true;

	private: // Non-serialized
		Transform* m_Transform = nullptr;
		glm::mat4 m_Projection;
		glm::mat4 m_InverseProjection;
		glm::mat4 m_View;
		glm::mat4 m_InverseView;
		float m_Width;
		float m_Height;
		CLASS_DECLARATION(Camera);
	};
}