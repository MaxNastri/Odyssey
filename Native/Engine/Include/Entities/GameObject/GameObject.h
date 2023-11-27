#pragma once
#include "Globals.h"
#include <ryml.hpp>

namespace Odyssey
{
	template<typename T>
	class RefHandle
	{
	public:
		template<typename... Args>
		static RefHandle<T> Create(uint32_t id, Args... params)
		{
			RefHandle<T> handle;
			handle.m_ID = id;
			handle.m_Ptr = std::make_shared<T>(params...);
			return handle;
		}

		static RefHandle<T> Empty()
		{
			RefHandle<T> handle;
			handle.m_ID = -1;
			handle.m_Ptr = nullptr;
			return handle;
		}

		operator uint32_t () const { return m_ID; }
		operator T* () const { return m_Ptr.get(); }

	public:
		uint32_t ID() { return m_ID; }
		T* Get() { return m_Ptr.get(); }
		std::shared_ptr<T> operator->() { return m_Ptr; }

	private:
		uint32_t m_ID;
		std::shared_ptr<T> m_Ptr;
	};

	class GameObject
	{
	public:
		GameObject();
		GameObject(uint32_t ID);
		void Serialize(ryml::NodeRef& node);
		void Deserialize(ryml::NodeRef& node);

	public:
		bool operator==(const GameObject& other) { return id == other.id; }

	public:
		bool active;
		uint32_t id;
		std::string name;
		std::string uuid;
		CLASS_DECLARATION(GameObject);
	};
}