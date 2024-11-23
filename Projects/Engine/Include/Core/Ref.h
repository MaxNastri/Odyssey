#pragma once

namespace Odyssey
{
	class RefCount
	{
	public:
		uint32_t Count() const { return m_Count.load(); }

		void Increment()
		{
			++m_Count;
		}

		void Decrement()
		{
			--m_Count;
		}

	private:
		std::atomic<uint32_t> m_Count = 0;
	};

	template<typename T>
	class Ref
	{
	public:
		Ref()
			: m_Instance(nullptr)
		{

		}

		Ref(std::nullptr_t n)
			: m_Instance(nullptr)
		{

		}

		Ref(T* instance)
			: m_Instance(instance)
		{
			m_RefCount = new RefCount();
			Increment();
		}

		~Ref()
		{
			Decrement();
		}

	public:
		Ref(const Ref<T>& other)
		{
			m_Instance = other.m_Instance;
			m_RefCount = other.m_RefCount;
			Increment();
		}

		template<typename T2>
		Ref(const Ref<T2>& other)
		{
			m_Instance = (T*)other.m_Instance;
			m_RefCount = other.m_RefCount;
			Increment();
		}

		template<typename T2>
		Ref(Ref<T2>&& other)
		{
			m_Instance = (T*)other.m_Instance;
			m_RefCount = other.m_RefCount;
			other.m_Instance = nullptr;
			other.m_RefCount = nullptr;
		}

		template<typename T2>
		Ref& operator=(const Ref<T2>& other)
		{
			other.m_RefCount->Increment();
			Decrement();

			m_Instance = other.m_Instance;
			m_RefCount = other.m_RefCount;
			return *this;
		}

		template<typename T2>
		Ref& operator=(Ref<T2>&& other)
		{
			Decrement();
			m_Instance = other.m_Instance;
			m_RefCount = other.m_RefCount;
			other.m_Instance = nullptr;
			other.m_RefCount = nullptr;
			return *this;
		}

		Ref& operator=(std::nullptr_t)
		{
			Decrement();
			m_Instance = nullptr;
			m_RefCount = nullptr;
			return *this;
		}

		Ref& operator=(const Ref<T>& other)
		{
			// Check yo self
			if (this == &other)
				return *this;

			other.m_RefCount->Increment();
			Decrement();

			m_Instance = other.m_Instance;
			m_RefCount = other.m_RefCount;

			return *this;
		}

	public:
		T& operator*() { return *m_Instance; }
		const T& operator*() const { return *m_Instance; }

		T* operator->() { return m_Instance; }
		const T* operator->() const { return m_Instance; }

		operator bool() { return m_Instance != nullptr; }
		operator bool() const { return m_Instance != nullptr; }

		bool operator==(const Ref<T>& other) const { return m_Instance == other.m_Instance; }
		bool operator!=(const Ref<T>& other) const { return !(*this == other); }

		bool operator() (const Ref& lhs, const Ref& rhs) const { return lhs.m_Instance < rhs.m_Instance; }
		bool operator<(const Ref& other) const { return m_Instance < other.m_Instance; }

	public:
		T* Get() { return m_Instance; }

	public:
		template<typename T2>
		Ref<T2> As() const
		{
			return Ref<T2>(*this);
		}

	private:
		void Increment()
		{
			if (m_RefCount)
				m_RefCount->Increment();
		}

		void Decrement()
		{
			if (m_RefCount)
			{
				m_RefCount->Decrement();

				if (m_RefCount->Count() == 0)
				{
					delete m_Instance;
					delete m_RefCount;
					m_Instance = nullptr;
					m_RefCount = nullptr;
				}
			}
		}

	private:
		template<typename T2>
		friend class Ref;

		mutable T* m_Instance = nullptr;
		mutable RefCount* m_RefCount = nullptr;
	};
}