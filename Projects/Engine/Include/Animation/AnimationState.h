#pragma once
#include "Rune.h"
#include "GUID.h"
#include "AnimationProperty.hpp"

namespace Odyssey
{
	namespace Rune
	{
		struct Node;
	}
	using namespace Rune;

	enum class ComparisonOp
	{
		Less = 0,
		LessOrEqual = 1,
		Equal = 2,
		Greater = 3,
		GreaterOrEqual = 4,
	};

	class AnimationState;

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
		AnimationState(std::shared_ptr<Node> node);

	public:
		AnimationState* Evaluate();

	public:
		std::string_view GetName();
		GUID GetClip();

	public:
		void SetClip(GUID guid) { m_AnimationClip = guid; }

	public:
		std::shared_ptr<AnimationLink> AddLink(AnimationState* connectedState, std::shared_ptr<AnimationProperty> property, ComparisonOp compareOp, RawBuffer targetValue);

	private:
		std::shared_ptr<Node> m_Node;
		std::vector<std::shared_ptr<AnimationLink>> m_Links;
		GUID m_AnimationClip;
	};
}