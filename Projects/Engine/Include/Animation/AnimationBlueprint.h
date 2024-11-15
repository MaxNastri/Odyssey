#pragma once
#include "Blueprint.h"
#include "Asset.h"
#include "AnimationProperty.hpp"
#include "BoneKeyframe.hpp"

namespace Odyssey
{
	using namespace Rune;

	struct AnimationProperty;
	class AnimationState;
	struct AnimationStateNode;

	class AnimationBlueprint : public Blueprint, public Asset
	{
		CLASS_DECLARATION(Odyssey, AnimationBlueprint)
	public:
		AnimationBlueprint();
		AnimationBlueprint(const Path& assetPath);

	public:
		void Save();
		void Load();

	private:
		void LoadFromDisk();
		void SaveToDisk(const Path& assetPath);

	public:
		virtual void Update() override;

	public:
		const std::map<std::string, BlendKey>& GetKeyframe();

	public:
		std::shared_ptr<AnimationStateNode> AddAnimationState(std::string name);

	public:
		std::vector<std::shared_ptr<AnimationProperty>>& GetProperties() { return m_Properties; }
		std::shared_ptr<AnimationState> GetAnimationState(GUID nodeGUID);

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
		std::map<GUID, std::shared_ptr<AnimationState>> m_States;
		std::shared_ptr<AnimationState> m_CurrentState;
	};
}