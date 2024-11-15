#pragma once
#include "Rune.h"
#include "GUID.h"
#include "AnimationProperty.hpp"
#include "BoneKeyframe.hpp"

namespace Odyssey
{
	namespace Rune
	{
		struct Node;
	}
	using namespace Rune;


	inline static const std::array<std::string, 5> Comparison_Op_Display =
	{
		"<", "<=", "=", ">", ">="
	};

	enum class ComparisonOp
	{
		Less = 0,
		LessOrEqual = 1,
		Equal = 2,
		Greater = 3,
		GreaterOrEqual = 4,
	};

	class AnimationState;
	class AnimationClip;

	class AnimationLink
	{
	public:
		AnimationLink(AnimationState* start, AnimationState* end, std::shared_ptr<AnimationProperty> property, ComparisonOp compareOp, RawBuffer targetValue);

	public:
		bool Evaluate();

	public:
		AnimationState* GetEndState() { return m_End; }

	private:
		AnimationState* m_Start;
		AnimationState* m_End;

		std::shared_ptr<AnimationProperty> m_Property;
		ComparisonOp m_CompareOp;
		RawBuffer m_TargetValue;

		Link* m_Link;
	};

	class AnimationState
	{
	public:
		AnimationState() = default;
		AnimationState(std::string_view name);
		AnimationState(std::string_view name, GUID animationClip);

	public:
		const std::map<std::string, BlendKey>& Evaluate();

	public:
		std::string_view GetName();
		std::shared_ptr<AnimationClip> GetClip();

	public:
		void SetClip(GUID guid);

	public:
		std::shared_ptr<AnimationLink> AddLink(AnimationState* connectedState, std::shared_ptr<AnimationProperty> property, ComparisonOp compareOp, RawBuffer targetValue);

	private:
		std::vector<std::shared_ptr<AnimationLink>> m_Links;
		std::shared_ptr<AnimationClip> m_AnimationClip;
		std::string m_Name;
	};
}