#pragma once
#include "Blueprint.h"

namespace Odyssey
{
	using namespace Rune;

	struct AnimationProperty;

	class AnimationBlueprint : public Rune::Blueprint
	{
	public:
		AnimationBlueprint();

	public:
		virtual void Update() override;
		virtual void Draw() override;

	public:
		virtual void AddLink(Pin* start, Pin* end) override;

	private:
		Pin* m_PendingLinkStart = nullptr;
		Pin* m_PendingLinkEnd = nullptr;

	public:
		bool SetBool(const std::string& name, bool value);
		bool SetFloat(const std::string& name, float value);
		bool SetInt(const std::string& name, int32_t value);
		bool SetTrigger(const std::string& name);

	private:
		void ClearTriggers();
		void EvalulateGraph();
		void DrawPropertiesPanel();
		void DrawAddAnimationLinkPopup();

	private:
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
		uint32_t m_AddLinkMenuID = 118;
	};
}