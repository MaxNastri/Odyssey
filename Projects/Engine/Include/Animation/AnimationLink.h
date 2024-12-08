#pragma once
#include "GUID.h"
#include "RawBuffer.h"
#include "Ref.h"
#include "AnimationState.h"
#include "AnimationProperty.h"

namespace Odyssey
{
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

	class AnimationCondition
	{
	public:
		AnimationCondition() = default;
		AnimationCondition(Ref<AnimationProperty> property, ComparisonOp comparison, const RawBuffer& targetValue);

	public:
		bool Evaluate();

	public:
		const std::string& GetPropertyName() { return m_Property->Name; }
		AnimationPropertyType GetPropertyType() { return m_Property->Type; }
		ComparisonOp GetComparison() { return m_ComparisonOp; }

	public:
		template<typename T>
		T GetTargetValue() { return m_TargetValue.Read<T>(); }

		template<typename T>
		void SetTargetValue(T value) { m_TargetValue.Write(&value); }

	private:
		Ref<AnimationProperty> m_Property;
		ComparisonOp m_ComparisonOp;
		RawBuffer m_TargetValue;
	};

	class AnimationLink
	{
	public:
		AnimationLink(Ref<AnimationState> start, Ref<AnimationState> end);
		AnimationLink(GUID guid, Ref<AnimationState> start, Ref<AnimationState> end);

	public:
		bool Evaluate(Ref<AnimationState>& currentState);

	public:
		void AddTransition(Ref<AnimationState> begin, Ref<AnimationState> end, Ref<AnimationCondition>& condition);

	public:
		GUID GetGUID() { return m_GUID; }
		Ref<AnimationState> GetBeginState() { return m_BeginState; }
		Ref<AnimationState> GetEndState() { return m_EndState; }
		const std::vector<Ref<AnimationCondition>>& GetForwardTransitions() { return m_ForwardTransitions; }
		const std::vector<Ref<AnimationCondition>>& GetReturnTransitions() { return m_ReturnTransitions; }
	
	private:
		GUID m_GUID;
		Ref<AnimationState> m_BeginState;
		Ref<AnimationState> m_EndState;

		std::vector<Ref<AnimationCondition>> m_ForwardTransitions;
		std::vector<Ref<AnimationCondition>> m_ReturnTransitions;
	};
}