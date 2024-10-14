#pragma once

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

	typedef uintptr_t PinId;

	struct Pin
	{
	public:
		PinId ID;
		std::string Name;
		Node* Node;
		PinIO IO;
		PinType Type;

	public:
		Pin(PinId id, std::string_view name, PinType type)
			: ID(id), Name(name), Node(nullptr), IO(PinIO::Input), Type(type)
		{

		}
	};
}