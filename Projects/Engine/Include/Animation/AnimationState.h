#pragma once
#include "Rune.h"
#include "AnimationProperty.hpp"

namespace Odyssey
{
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

		Rune::Link* m_Link;
	};

	class AnimationState
	{
	public:
		AnimationState(Rune::Node* node);

	public:
		AnimationState* Evaluate();

	public:
		std::shared_ptr<AnimationLink> AddLink(AnimationState* connectedState, std::shared_ptr<AnimationProperty> property, ComparisonOp compareOp, RawBuffer targetValue);

	private:
		Rune::Node* m_Node;
		std::vector<std::shared_ptr<AnimationLink>> m_Links;
	};
}