#pragma once
#include "Blueprint.h"
#include "Asset.h"
#include "AnimationLink.h"
#include "AnimationState.h"
#include "BoneKeyframe.h"
#include "RawBuffer.h"
#include "Ref.h"

namespace Odyssey
{
	using namespace Rune;

	struct AnimationProperty;
	struct AnimationStateNode;

	class AnimationBlueprint : public Asset, public Blueprint
	{
		CLASS_DECLARATION(Odyssey, AnimationBlueprint)
	public:
		AnimationBlueprint();
		AnimationBlueprint(const Path& assetPath);

	public:
		virtual void Save() override;
		void Load();

	private:
		void LoadFromDisk();
		void SaveToDisk(const Path& assetPath);

	public:
		virtual void Update() override;

	public:
		const std::map<std::string, BlendKey>& GetKeyframe();

	public:
		Ref<AnimationStateNode> AddAnimationState(std::string name);

	public:
		Ref<AnimationProperty> GetProperty(const std::string& propertyName);
		std::vector<Ref<AnimationProperty>>& GetProperties() { return m_Properties; }
		Ref<AnimationState> GetAnimationState(GUID nodeGUID);
		Ref<AnimationLink> GetAnimationLink(GUID linkGUID);
		std::vector<std::string> GetAllPropertyNames();

	public:
		void AddProperty(std::string_view name, AnimationPropertyType type);
		bool SetBool(const std::string& name, bool value);
		bool SetFloat(const std::string& name, float value);
		bool SetInt(const std::string& name, int32_t value);
		bool SetTrigger(const std::string& name);

	public:
		void AddAnimationLink(GUID startNode, GUID endNode, int32_t propertyIndex, ComparisonOp comparisonOp, RawBuffer& propertyValue);

	private:
		void ClearTriggers();
		Ref<AnimationProperty> GetProperty(std::string_view name);

	private:
		std::vector<Ref<AnimationProperty>> m_Properties;
		std::map<GUID, Ref<AnimationState>> m_States;
		std::map<Ref<AnimationState>, std::vector<Ref<AnimationLink>>> m_StateToLinks;
		Ref<AnimationState> m_CurrentState;
		Ref<AnimationState> m_PrevState;
		float m_CurrentBlendTime = 0.0f;
		float m_EndBlendTime = 0.0f;
	};
}