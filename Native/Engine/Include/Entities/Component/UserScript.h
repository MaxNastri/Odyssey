#pragma once
#include "Component.h"
#include <ManagedObject.hpp>
#include <Type.hpp>

namespace Odyssey
{
	class UserScript : public Component
	{
	public:
		UserScript() = default;
		UserScript(const std::string& managedType);
		virtual void Awake() override;
		virtual void Update() override;
		virtual void OnDestroy() override;
		virtual void Serialize(ryml::NodeRef& node) override;
		virtual void Deserialize(ryml::ConstNodeRef node) override;

	public:
		Coral::Type GetType() { return managedInstance.GetType(); }
		Coral::ManagedObject GetManagedObject() { return managedInstance; }
		std::string GetManagedTypeName() { return m_ManagedType; }

	public:
		void SetManagedInstance(Coral::ManagedObject instance);
		void SetManagedType(std::string_view managedClassName);

	private: // yaml
		bool SerializeNativeTypes(const Coral::ManagedType& managedType, const std::string& fieldName, ryml::NodeRef node);
		bool SerializeNativeString(const std::string& fieldName, ryml::NodeRef& node);
		bool DeserializeNativeType(const Coral::ManagedType& managedType, const std::string& fieldName, ryml::ConstNodeRef node);
		bool DeserializeNativeString(const std::string& fieldName, ryml::ConstNodeRef node);
	private:
		Coral::ManagedObject managedInstance;
		std::string m_ManagedType;
		CLASS_DECLARATION(UserScript);
	};
}