#pragma once
#include "Blueprint.h"
#include "AnimationProperty.hpp"

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

	public:
		std::vector<std::shared_ptr<AnimationProperty>>& GetProperties() { return m_Properties; }

	private:
		Pin* m_PendingLinkStart = nullptr;
		Pin* m_PendingLinkEnd = nullptr;

	public:
		void AddProperty(std::string_view name, AnimationPropertyType type);
		bool SetBool(const std::string& name, bool value);
		bool SetFloat(const std::string& name, float value);
		bool SetInt(const std::string& name, int32_t value);
		bool SetTrigger(const std::string& name);

	private:
		void ClearTriggers();
		void EvalulateGraph();

	private:
		std::vector<std::shared_ptr<AnimationProperty>> m_Properties;
		std::unordered_map<std::string, std::shared_ptr<AnimationProperty>> m_PropertyMap;
	};
}