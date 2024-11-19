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
		AnimationLink(std::shared_ptr<AnimationState> start, std::shared_ptr<AnimationState> end, std::shared_ptr<AnimationProperty> property, ComparisonOp compareOp, RawBuffer& targetValue);

	public:
		bool Evaluate();

	public:
		std::shared_ptr<AnimationState> GetBeginState() { return m_BeginState; }
		std::shared_ptr<AnimationState> GetEndState() { return m_EndState; }
		std::shared_ptr<AnimationProperty> GetProperty() { return m_Property; }
		ComparisonOp GetComparisonOp() { return m_ComparisonOp; }
		RawBuffer& GetTargetValue() { return m_TargetValue; }

	private:
		std::shared_ptr<AnimationState> m_BeginState;
		std::shared_ptr<AnimationState> m_EndState;

		std::shared_ptr<AnimationProperty> m_Property;
		ComparisonOp m_ComparisonOp;
		RawBuffer m_TargetValue;

		Link* m_Link;
	};

	class AnimationState
	{
	public:
		AnimationState() = default;
		AnimationState(std::string_view name);
		AnimationState(GUID guid, std::string_view name, GUID animationClip);

	public:
		const std::map<std::string, BlendKey>& Evaluate();

	public:
		std::string_view GetName();
		std::shared_ptr<AnimationClip> GetClip();

	public:
		GUID GetGUID() { return m_GUID; }
		void SetClip(GUID guid);

	private:
		std::shared_ptr<AnimationClip> m_AnimationClip;
		std::string m_Name;
		GUID m_GUID;
	};
}