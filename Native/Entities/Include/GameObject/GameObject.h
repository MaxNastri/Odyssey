#pragma once

namespace Odyssey::Entities
{
	class GameObject
	{
	public:
		GameObject(unsigned int ID)
		{
			id = ID;
			active = true;
		}

	public:
		bool active;
		unsigned int id;
	};
}