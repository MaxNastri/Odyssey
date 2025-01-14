#pragma once
#include "AssetSerializer.h"
#include "GameObject.h"

namespace Odyssey
{
	class Transform;

	class Camera
	{
	private:
		CLASS_DECLARATION(Odyssey, Camera)

	public:
		enum class Tag : uint8_t
		{
			None = 0,
			Main = 1,
			SceneView = 2,
			Custom = 3,
		};
	public:
		Camera() = default;
		Camera(const GameObject& gameObject);
		Camera(const GameObject& gameObject, SerializationNode& node);

	public:
		void Awake();

	public:
		void Serialize(SerializationNode& node);
		void Deserialize(SerializationNode& node);

	public:
		bool IsEnabled() { return m_Enabled; }
		bool IsMainCamera() { return m_MainCamera; }

	public:
		GameObject& GetGameObject() { return m_GameObject; }
		Camera::Tag GetTag() { return m_Tag; }
		float GetFieldOfView() { return m_FieldOfView; }
		float GetNearClip() { return m_NearClip; }
		float GetFarClip() { return m_FarClip; }
		float GetViewportWidth() { return m_Width; }
		float GetViewportHeight() { return m_Height; }

	public:
		mat4 GetProjection() { return m_Projection; }
		mat4 GetInverseProjection() { return m_InverseProjection; }
		mat4 GetScreenSpaceProjection();
		mat4 GetView();
		mat4 GetInverseView();
		float4 GetViewPosition();

	public:
		void SetMainCamera(bool mainCamera) { m_MainCamera = mainCamera; }
		void SetEnabled(bool enabled);
		void SetTag(Camera::Tag tag) { m_Tag = tag; }
		void SetFieldOfView(float fov);
		void SetNearClip(float nearClip);
		void SetFarClip(float farClip);
		void SetViewportSize(float width, float height);

	private:
		void CalculateProjection();
		void CalculateInverseView();

	private: // Serialized
		bool m_Enabled = true;
		float m_FieldOfView = 45.0f;
		float m_NearClip = 0.1f;
		float m_FarClip = 1000.0f;
		bool m_MainCamera = true;
		Tag m_Tag = Tag::None;

	private: // Non-serialized
		GameObject m_GameObject;
		glm::mat4 m_Projection;
		glm::mat4 m_InverseProjection;
		glm::mat4 m_View;
		glm::mat4 m_InverseView;
		float m_Width = 1920;
		float m_Height = 1080;
	};
}