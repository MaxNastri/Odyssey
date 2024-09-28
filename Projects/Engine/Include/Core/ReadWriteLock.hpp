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
		ReadWriteLock()
		{
			InitializeSRWLock(&m_Lock);
		}

	public:
		void Lock(LockState state)
		{
			if (state == LockState::Read)
				AcquireSRWLockShared(&m_Lock);
			else if (state == LockState::Write)
				AcquireSRWLockExclusive(&m_Lock);
		}

		void Unlock(LockState state)
		{
			if (state == LockState::Read)
				ReleaseSRWLockShared(&m_Lock);
			else if (state == LockState::Write)
				ReleaseSRWLockExclusive(&m_Lock);
		}

	private:
		SRWLOCK m_Lock;
		LockState m_State = LockState::None;
	};
}