#pragma once

namespace Odyssey
{
	enum class LockState
	{
		None = 0,
		Read = 1,
		Write = 2
	};

	class ReadWriteLock
	{
	public:
		ReadWriteLock();

	public:
		void Lock(LockState state);
		void Unlock(LockState state);

	private:
		SRWLOCK m_Lock;
		LockState m_State = LockState::None;
	};
}