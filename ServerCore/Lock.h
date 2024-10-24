#pragma once
#include "Types.h"

/*-------------------
*	RW SpinLock
-------------------*/

/*-----------------------------------------------
[WWWWWWWW] [WWWWWWWW]	[RRRRRRRR] [RRRRRRRR]
W: WriteFlag (Exclusive Lock Owner ThreadID)
R: ReadFlag (Shared Lock Count)
-----------------------------------------------*/

//동일 스레드에서
// W W W W W W W W W W -> R 가능
// R -> W (X) 불가능
class Lock
{
	enum : uint32
	{
		ACQUIRE_TIMEOUT_TICK = 10000,
		MAX_SPIN_COUNT = 5000,
		WRITE_THREAD_MASK = 0XFFFF'0000,
		READ_COUNT_MASK = 0X0000'FFFF,
		EMPTY_FLAG = 0X0000'0000,
	};

public:
	void WriteLock();
	void WriteUnlock();
	void RaedLock();
	void ReadUnlock();

private:
	Atomic<uint32> _lockFag = EMPTY_FLAG;
	uint16 _writeCount = 0;
};

/*-------------------
*	LockGuards
-------------------*/

class ReadLockGuard
{
public:
	ReadLockGuard(Lock& lock)
		: _lock(lock)
	{
		_lock.RaedLock();
	}

	~ReadLockGuard()
	{
		_lock.ReadUnlock();
	}

private:
	Lock& _lock;
};

class WriteLockGuard
{
public:
	WriteLockGuard(Lock& lock)
		: _lock(lock)
	{
		_lock.WriteLock();
	}

	~WriteLockGuard()
	{
		_lock.WriteUnlock();
	}

private:
	Lock& _lock;
};