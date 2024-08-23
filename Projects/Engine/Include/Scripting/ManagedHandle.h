#pragma once
#include "ManagedObject.hpp"

namespace Odyssey
{
	class ManagedHandle
	{
	public:
		ManagedHandle() = default;
		ManagedHandle(Coral::ManagedObject* object) : m_Object(object)
		{

		}
	public:
		template<typename... Args>
		void Invoke(std::string_view function, Args&&... args)
		{
			m_Object->InvokeMethod(function, std::forward<Args>(args)...);
		}

		bool IsValid() { return m_Object; }
		Coral::ManagedObject* GetManagedObject() const { return m_Object; }
		void Clear() { m_Object = nullptr; }
	private:
		Coral::ManagedObject* m_Object;
	};
}