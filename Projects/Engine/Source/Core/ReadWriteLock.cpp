#include "ReadWriteLock.h"

namespace Odyssey
{
	ReadWriteLock::ReadWriteLock()
	{
		InitializeSRWLock(&m_Lock);
	}

	void ReadWriteLock::Lock(LockState state)
	{
		if (state == LockState::Read)
			AcquireSRWLockShared(&m_Lock);
		else if (state == LockState::Write)
			AcquireSRWLockExclusive(&m_Lock);
	}

	void ReadWriteLock::Unlock(LockState state)
	{
		if (state == LockState::Read)
			ReleaseSRWLockShared(&m_Lock);
		else if (state == LockState::Write)
			ReleaseSRWLockExclusive(&m_Lock);
	}
}