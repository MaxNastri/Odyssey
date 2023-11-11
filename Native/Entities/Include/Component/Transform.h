#pragma once
#include "Component.h"
#include <Vector4.h>
#include <Vector3.h>
#include <Quaternion.h>
#include <Matrix4x4.h>
#include <Serialization.h>

namespace Odyssey::Entities
{
	class Transform : public Component
	{
	public:
		virtual void Awake() override;
		virtual void Update() override;
		virtual void OnDestroy() override;
		virtual void Serialize(json& jsonObject) override;
		virtual void Deserialize(const json& jsonObject) override;

    public:
        void AddPosition(Vector3 pos);
        void AddPosition(float x, float y, float z);
        void SetPosition(Vector3 pos);
        void SetPosition(float x, float y, float z);
        void AddRotation(Vector3 eulerAngles);
        void AddRotation(Quaternion orientation);
        void SetRotation(Vector3 eulerAngles);
        void SetRotation(Quaternion orientation);
        void AddScale(Vector3 scaleFactor);
        void AddScale(float x, float y, float z);
        void SetScale(Vector3 scaleFactor);
        void SetScale(float x, float y, float z);

        Vector3 GetPosition() { return position; }
        Vector3 GetEulerRotation() { return eulerRotation; }
        Quaternion GetRotation() { return rotation; }
        Vector3 GetScale() { return scale; }
        Vector3 Forward();
        Vector3 Right();
        Vector3 Up();
        Matrix4x4 GetWorldMatrix();

    public:
        void UpdateWorldMatrix(bool forceUpdateWorld = false);
        void RotateAround(Vector3 center, Vector3 axis, float degrees, bool worldSpace);
    
    private:
        void CalculateEulerRotations();
        void ComposeLocalMatrix();
        void Reset();

	public:
		Vector3 position;
		Vector3 eulerRotation;
		Quaternion rotation;
		Vector3 scale;
        Matrix4x4 localMatrix;
        Matrix4x4 worldMatrix;
        bool dirty;
		CLASS_DECLARATION(Transform);
		ODYSSEY_SERIALIZE(Transform, Type);
	};
}