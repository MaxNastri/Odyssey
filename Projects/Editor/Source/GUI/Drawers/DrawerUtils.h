#pragma once
#include "StringDrawer.h"
#include "BoolDrawer.h"
#include "IntDrawer.h"
#include "FloatDrawer.h"
#include "DoubleDrawer.h"
#include "ScriptComponent.h"
#include "GameObject.h"
#include "ManagedObject.hpp"

namespace Odyssey
{
	template <typename FieldType>
	std::shared_ptr<IntDrawer<FieldType>> AddIntDrawer(GameObject& gameObject, const std::string& fieldName, FieldType initialValue)
	{
		std::function<void(FieldType)> callback = [&gameObject, fieldName](FieldType fieldValue)
			{
				if (ScriptComponent* userScript = gameObject.TryGetComponent<ScriptComponent>())
				userScript->GetManagedObject().SetFieldValue<FieldType>(fieldName, fieldValue);
			};

		return std::make_shared<IntDrawer<FieldType>>(fieldName, initialValue, callback);
	}

	std::shared_ptr<BoolDrawer> AddBoolDrawer(GameObject& gameObject, const std::string& fieldName, bool initialValue)
	{
		std::function<void(bool)> callback = [&gameObject, fieldName](bool fieldValue)
			{
				if (ScriptComponent* userScript = gameObject.TryGetComponent<ScriptComponent>())
					userScript->GetManagedObject().SetFieldValue<uint32_t>(fieldName, fieldValue);
			};

		return std::make_shared<BoolDrawer>(fieldName, initialValue, callback);
	}

	std::shared_ptr<FloatDrawer> AddFloatDrawer(GameObject& gameObject, const std::string& fieldName, float initialValue)
	{
		std::function<void(float)> callback = [&gameObject, fieldName](float fieldValue)
			{
				if (ScriptComponent* userScript = gameObject.TryGetComponent<ScriptComponent>())
					userScript->GetManagedObject().SetFieldValue<float>(fieldName, fieldValue);
			};

		return std::make_shared<FloatDrawer>(fieldName, initialValue, callback);
	}

	std::shared_ptr<DoubleDrawer> AddDoubleDrawer(GameObject& gameObject, const std::string& fieldName, double initialValue)
	{
		std::function<void(double)> callback = [&gameObject, fieldName](double fieldValue)
			{
				if (ScriptComponent* userScript = gameObject.TryGetComponent<ScriptComponent>())
				{
					userScript->GetManagedObject().SetFieldValue<double>(fieldName, fieldValue);
				}
			};

		return std::make_shared<DoubleDrawer>(fieldName, initialValue, callback);
	}

	std::shared_ptr<StringDrawer> AddStringDrawer(GameObject& gameObject, const std::string& fieldName, std::string initialValue)
	{
		std::function<void(std::string)> callback = [&gameObject, fieldName](std::string fieldValue)
			{
				if (ScriptComponent* userScript = gameObject.TryGetComponent<ScriptComponent>())
				{
					Coral::ScopedString field = Coral::String::New(fieldValue);
					userScript->GetManagedObject().SetFieldValue<Coral::String>(fieldName, field);
				}
			};

		return std::make_shared<StringDrawer>(fieldName, initialValue, callback);
	}
}