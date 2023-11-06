#pragma once
#include "Component.h"
#include <ManagedObject.hpp>

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

	public:
		void SetManagedInstance(Coral::ManagedObject instance);

	private:
		bool SerializeNativeTypes(const Coral::ManagedType& managedType, const std::string& fieldName, json& jsonObject);
		bool DeserializeNativeType(const Coral::ManagedType& managedType, const std::string& fieldName, const json& jsonObject);
	private:
		Coral::ManagedObject managedInstance;
		CLASS_DECLARATION(UserScript);
		ODYSSEY_SERIALIZE(UserScript, Type);
	};
}