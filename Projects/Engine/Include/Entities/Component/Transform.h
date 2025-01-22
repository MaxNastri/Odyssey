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
        Transform(const GameObject& gameObject, SerializationNode& node);

	public:
        void Awake() { }
        void Serialize(SerializationNode& node);
        void Deserialize(SerializationNode& node);

    public:
        void SetPosition(float3 position);
        void SetPosition(float x, float y, float z);
        void SetRotation(float3 eulerAngles);
        void SetRotation(quat orientation);
        void SetScale(float3 scaleFactor);
        void SetScale(float x, float y, float z);

        void SetLocalMatrix(mat4 localMatrix);

        float3 GetPosition() { return m_Position; }
        float3 GetWorldPosition();

        float3 GetEulerRotation() { return m_EulerRotation; }
        quat GetRotation() { return m_Rotation; }
        float3 GetScale() { return m_Scale; }
        float3 Forward();
        float3 Right();
        float3 Up();
        mat4 GetLocalMatrix();
        mat4 GetWorldMatrix();

    public:
        void SetLocalSpace();
        void DecomposeWorldMatrix(float3& pos, quat& rotation, float3& scale);
        void RotateAround(float3 center, float3 axis, float degrees, bool worldSpace);
        void LookAt(float3 center, float3 up);

    private:
        void CalculateEulerRotations();
        void ComposeLocalMatrix();
        void Reset();

	public:
        GameObject m_GameObject;
		float3 m_Position;
        float3 m_WorldPosition;
        quat m_Rotation;
        quat m_WorldRotation;
        float3 m_Scale;
        float3 m_WorldScale;
        float3 m_EulerRotation;
        mat4 m_LocalMatrix;
        mat4 m_WorldMatrix;
        bool m_Dirty;
		CLASS_DECLARATION(Odyssey, Transform)
	};
}