#pragma once

namespace Odyssey::Entities
{
	class GameObject
	{
	public:
		GameObject()
		{
			id = -1;
			active = false;
		}

		GameObject(unsigned int ID)
		{
			id = ID;
			active = true;
		}

		bool operator==(const GameObject& other) { return id == other.id; }
	public:
		bool active;
		unsigned int id;
	};
}