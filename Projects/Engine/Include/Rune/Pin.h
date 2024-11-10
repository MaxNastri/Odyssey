#pragma once
#include "GUID.h"

namespace Odyssey::Rune
{
	enum class PinType
	{
		None = 0,
		Flow = 1,
		Bool = 2,
		Int = 3,
		Float = 4,
		String = 5,
		Object = 6,
		Function = 7,
		Delegate = 8,
	};

	enum class PinIO
	{
		None = 0,
		Input = 1,
		Output = 2,
	};

	struct Node;

	struct Pin
	{
	public:
		GUID Guid;
		std::string Name;
		Node* Node;
		PinIO IO;
		PinType Type;
		bool Linked = false;
		bool Modifiable = true;

	public:
		Pin(std::string_view name, PinType type, bool modifiable = true);

	public:
		static bool CanCreateLink(Pin* a, Pin* b);

	public:
		void Draw(float alpha = 1.0f);

	public:
		void Modify(std::string_view name, PinType type);

	public:
		float3 GetColor() { return Pin_Colors[Type]; }

	private:
		inline static constexpr float2 Icon_Size = float2(24.0f);
		inline static constexpr float3 Inner_Color = float3(0.12f);
		inline static std::unordered_map<PinType, float3> Pin_Colors =
		{
			{ PinType::Flow, float3(1.0f, 1.0f, 1.0f) },
			{ PinType::Bool, float3(0.86f, 0.18f, 0.18f) },
			{ PinType::Int, float3(0.26f, 0.79f, 0.61f) },
			{ PinType::Float, float3(0.57f, 0.88f, 0.29f) },
			{ PinType::String, float3(0.48f, 0.08f, 0.6f) },
			{ PinType::Object, float3(0.2f, 0.59f, 0.84f) },
			{ PinType::Function, float3(0.86f, 0.0f, 0.72f) },
			{ PinType::Delegate, float3(1.0f, 0.18f, 0.18f) },
		};
	};
}