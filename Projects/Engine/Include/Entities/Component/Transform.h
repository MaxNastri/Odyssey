#pragma once
#include "AssetSerializer.h"
#include "GameObject.h"

namespace Odyssey
{
	class Transform
	{
    public:
        Transform() = default;
        Transform(const GameObject& gameObject);

	public:
        void Serialize(SerializationNode& node);
        void Deserialize(SerializationNode& node);

    public:
        void AddPosition(glm::vec3 pos);
        void AddPosition(float x, float y, float z);
        void SetPosition(glm::vec3 pos);
        void SetPosition(float x, float y, float z);
        void AddRotation(glm::vec3 eulerAngles);
        void AddRotation(glm::quat orientation);
        void SetRotation(glm::vec3 eulerAngles);
        void SetRotation(glm::quat orientation);
        void AddScale(glm::vec3 scaleFactor);
        void AddScale(float x, float y, float z);
        void SetScale(glm::vec3 scaleFactor);
        void SetScale(float x, float y, float z);

        void SetLocalMatrix(glm::mat4 localMatrix);

        glm::vec3 GetPosition() { return m_Position; }
        glm::vec3 GetEulerRotation() { return m_EulerRotation; }
        glm::quat GetRotation() { return m_Rotation; }
        glm::vec3 GetScale() { return m_Scale; }
        glm::vec3 Forward();
        glm::vec3 Right();
        glm::vec3 Up();
        glm::mat4 GetLocalMatrix();
        glm::mat4x4 GetWorldMatrix();

    public:
        void RotateAround(glm::vec3 center, glm::vec3 axis, float degrees, bool worldSpace);
        void LookAt(float3 center, float3 up);

    private:
        void CalculateEulerRotations();
        void ComposeLocalMatrix();
        void Reset();

	public:
        GameObject m_GameObject;
		glm::vec3 m_Position;
		glm::vec3 m_EulerRotation;
        glm::quat m_Rotation;
        glm::vec3 m_Scale;
        glm::mat4x4 m_LocalMatrix;
        glm::mat4x4 m_WorldMatrix;
        bool m_Dirty;
		CLASS_DECLARATION(Odyssey, Transform)
	};
}