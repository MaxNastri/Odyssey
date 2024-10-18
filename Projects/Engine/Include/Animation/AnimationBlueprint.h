#pragma once
#include "Blueprint.h"
#include "RawBuffer.hpp"

namespace Odyssey
{
	using namespace Rune;

	enum class AnimationPropertyType
	{
		None = 0,
		Float = 1,
		Int = 2,
		Bool = 3,
		Trigger = 4,
	};

	struct AnimationProperty
	{
		std::string Name;
		AnimationPropertyType Type;
		RawBuffer ValueBuffer;

	public:
		AnimationProperty(std::string_view name, AnimationPropertyType type)
			: Name(name), Type(type)
		{
			switch (type)
			{
				case AnimationPropertyType::None:
					break;
				case AnimationPropertyType::Float:
					ValueBuffer.Allocate(sizeof(float));
					break;
				case AnimationPropertyType::Int:
					ValueBuffer.Allocate(sizeof(int32_t));
					break;
				case AnimationPropertyType::Bool:
					ValueBuffer.Allocate(sizeof(bool));
					break;
				case AnimationPropertyType::Trigger:
					ValueBuffer.Allocate(sizeof(bool));
					break;
				default:
					break;
			}
		}
	};

	class AnimationBlueprint : public Rune::Blueprint
	{
	public:
		AnimationBlueprint();

	public:
		virtual void Update() override;
		virtual void Draw() override;

	public:
		bool SetBool(const std::string& name, bool value);
		bool SetFloat(const std::string& name, float value);
		bool SetInt(const std::string& name, int32_t value);
		bool SetTrigger(const std::string& name);

	private:
		void ClearTriggers();
		void EvalulateGraph();
		void DrawPropertiesPanel();

	private:
		//std::map<std::string, Property> m_Properties;
		std::vector<std::shared_ptr<AnimationProperty>> m_Properties;
		std::map<std::string, std::shared_ptr<AnimationProperty>> m_PropertyMap;

	private:
		struct PropertiesPanel
		{
		public:
			float2 Size = float2(400.0f, 800.0f);
			float2 MinSize = float2(50.0f, 50.0f);
		};
		PropertiesPanel m_PropertiesPanel;

	private:
		const std::string Create_Node_Menu = "My Create Node";
		uint32_t m_CreateNodeMenuID = 117;
	};
}