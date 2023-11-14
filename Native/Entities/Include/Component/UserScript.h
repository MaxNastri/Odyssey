#pragma once
#include "Component.h"
#include <ManagedObject.hpp>
#include <Type.hpp>

namespace Odyssey::Entities
{
	class UserScript : public Component
	{
	public:
		UserScript() = default;
		UserScript(const std::string& managedType);
		virtual void Awake() override;
		virtual void Update() override;
		virtual void OnDestroy() override;
		virtual void Serialize(json& jsonObject) override;
		virtual void Deserialize(const json& jsonObject) override;
		virtual void Serialize(ryml::NodeRef& node) override;
		virtual void Deserialize(ryml::ConstNodeRef node) override;

	public:
		void SetManagedInstance(Coral::ManagedObject instance);
		Coral::Type GetType() { return managedInstance.GetType(); }
		Coral::ManagedObject GetManagedObject() { return managedInstance; }
		void SetManagedType(std::string_view managedClassName);

	private: //json
		bool SerializeNativeTypes(const Coral::ManagedType& managedType, const std::string& fieldName, json& jsonObject);
		bool SerializeNativeString(const std::string& fieldName, json& jsonObject);
		bool DeserializeNativeType(const Coral::ManagedType& managedType, const std::string& fieldName, const json& jsonObject);
		bool DeserializeNativeString(const std::string& fieldName, const json& jsonObject);

	private: // yaml
		bool SerializeNativeTypes(const Coral::ManagedType& managedType, const std::string& fieldName, ryml::NodeRef node);
		bool SerializeNativeString(const std::string& fieldName, ryml::NodeRef& node);
		bool DeserializeNativeType(const Coral::ManagedType& managedType, const std::string& fieldName, ryml::ConstNodeRef jsonObject);
		bool DeserializeNativeString(const std::string& fieldName, ryml::ConstNodeRef node);
	private:
		Coral::ManagedObject managedInstance;
		CLASS_DECLARATION(UserScript);
		ODYSSEY_SERIALIZE(UserScript, Type);
	};
}