#pragma once
#include "AssetSerializer.h"
#include "GameObject.h"
#include "FileID.h"

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

        glm::vec3 GetPosition() { return m_Position; }
        glm::vec3 GetEulerRotation() { return m_EulerRotation; }
        glm::quat GetRotation() { return m_Rotation; }
        glm::vec3 GetScale() { return m_Scale; }
        glm::vec3 Forward();
        glm::vec3 Right();
        glm::vec3 Up();
        glm::mat4x4 GetWorldMatrix();

    public:
        void UpdateWorldMatrix(bool forceUpdateWorld = false);
        void RotateAround(glm::vec3 center, glm::vec3 axis, float degrees, bool worldSpace);
    
    private:
        void CalculateEulerRotations();
        void ComposeLocalMatrix();
        void Reset();

	public:
        FileID m_FileID;
        GameObject m_GameObject;
		glm::vec3 m_Position;
		glm::vec3 m_EulerRotation;
        glm::quat m_Rotation;
        glm::vec3 m_Scale;
        glm::mat4x4 m_LocalMatrix;
        glm::mat4x4 m_WorldMatrix;
        bool m_Dirty;
		CLASS_DECLARATION(Transform);
	};
}