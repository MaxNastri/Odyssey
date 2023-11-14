#pragma once
#include "Component.h"
#include <Serialization.h>
#include <glm.h>

namespace Odyssey
{
	class Transform : public Component
	{
	public:
		virtual void Awake() override;
		virtual void Update() override;
		virtual void OnDestroy() override;
		virtual void Serialize(json& jsonObject) override;
		virtual void Deserialize(const json& jsonObject) override;
        virtual void Serialize(ryml::NodeRef& node) override;
        virtual void Deserialize(ryml::ConstNodeRef node) override;

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

        glm::vec3 GetPosition() { return position; }
        glm::vec3 GetEulerRotation() { return eulerRotation; }
        glm::quat GetRotation() { return rotation; }
        glm::vec3 GetScale() { return scale; }
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
		glm::vec3 position;
		glm::vec3 eulerRotation;
        glm::quat rotation;
        glm::vec3 scale;
        glm::mat4x4 localMatrix;
        glm::mat4x4 worldMatrix;
        bool dirty;
		CLASS_DECLARATION(Transform);
		ODYSSEY_SERIALIZE(Transform, Type);
	};
}