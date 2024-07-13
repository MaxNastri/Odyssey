#pragma once
#include "StringDrawer.h"
#include "BoolDrawer.h"
#include "IntDrawer.h"
#include "FloatDrawer.h"
#include "DoubleDrawer.h"
#include <GameObject.h>
#include <ManagedObject.hpp>
#include <string>
#include <vector>
#include <memory>

namespace Odyssey
{
	template <typename FieldType>
	void AddIntDrawer(GameObject* gameObject, const std::string& userScriptClassName, const std::string& fieldName, FieldType initialValue, std::vector<std::unique_ptr<PropertyDrawer>>& drawers)
	{
		std::function<void(FieldType)> callback = [gameObject, userScriptClassName, fieldName](FieldType fieldValue)
			{
				if (UserScript* userScript = gameObject->GetUserScript(userScriptClassName))
				{
					userScript->GetManagedObject().SetFieldValue<FieldType>(fieldName, fieldValue);
				}
			};

		std::unique_ptr<IntDrawer<FieldType>> drawer = std::make_unique<IntDrawer<FieldType>>(fieldName, initialValue, callback);
		drawers.push_back(std::move(drawer));
	}

	void AddBoolDrawer(GameObject* gameObject, const std::string& userScriptClassName, const std::string& fieldName, bool initialValue, std::vector<std::unique_ptr<PropertyDrawer>>& drawers)
	{
		std::function<void(bool)> callback = [gameObject, userScriptClassName, fieldName](bool fieldValue)
			{
				if (UserScript* userScript = gameObject->GetUserScript(userScriptClassName))
				{
					userScript->GetManagedObject().SetFieldValue<uint32_t>(fieldName, fieldValue);
				}
			};

		std::unique_ptr<BoolDrawer> drawer = std::make_unique<BoolDrawer>(fieldName, initialValue, callback);
		drawers.push_back(std::move(drawer));
	}

	void AddFloatDrawer(GameObject* gameObject, const std::string& userScriptClassName, const std::string& fieldName, float initialValue, std::vector<std::unique_ptr<PropertyDrawer>>& drawers)
	{
		std::function<void(float)> callback = [gameObject, userScriptClassName, fieldName](float fieldValue)
			{
				if (UserScript* userScript = gameObject->GetUserScript(userScriptClassName))
				{
					userScript->GetManagedObject().SetFieldValue<float>(fieldName, fieldValue);
				}
			};

		std::unique_ptr<FloatDrawer> drawer = std::make_unique<FloatDrawer>(fieldName, initialValue, callback);
		drawers.push_back(std::move(drawer));
	}

	void AddDoubleDrawer(GameObject* gameObject, const std::string& userScriptClassName, const std::string& fieldName, double initialValue, std::vector<std::unique_ptr<PropertyDrawer>>& drawers)
	{
		std::function<void(double)> callback = [gameObject, userScriptClassName, fieldName](double fieldValue)
			{
				if (UserScript* userScript = gameObject->GetUserScript(userScriptClassName))
				{
					userScript->GetManagedObject().SetFieldValue<double>(fieldName, fieldValue);
				}
			};

		std::unique_ptr<DoubleDrawer> drawer = std::make_unique<DoubleDrawer>(fieldName, initialValue, callback);
		drawers.push_back(std::move(drawer));
	}

	void AddStringDrawer(GameObject* gameObject, const std::string& userScriptClassName, const std::string& fieldName, std::string initialValue, std::vector<std::unique_ptr<PropertyDrawer>>& drawers)
	{
		std::function<void(std::string)> callback = [gameObject, userScriptClassName, fieldName](std::string fieldValue)
			{
				if (UserScript* userScript = gameObject->GetUserScript(userScriptClassName))
				{
					Coral::ScopedString field = Coral::String::New(fieldValue);
					userScript->GetManagedObject().SetFieldValue<Coral::String>(fieldName, field);
				}
			};

		std::unique_ptr<StringDrawer> drawer = std::make_unique<StringDrawer>(fieldName, initialValue, callback);
		drawers.push_back(std::move(drawer));
	}
}