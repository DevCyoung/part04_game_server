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
	void WriteLock(const char* name);
	void WriteUnlock(const char* name);
	void RaedLock(const char* name);
	void ReadUnlock(const char* name);

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
	ReadLockGuard(Lock& lock, const char* name)
		: _lock(lock)
		, _name(name)
	{
		_lock.RaedLock(_name);
	}

	~ReadLockGuard()
	{
		_lock.ReadUnlock(_name);
	}

private:
	Lock& _lock;
	const char* _name;
};

class WriteLockGuard
{
public:
	WriteLockGuard(Lock& lock, const char* name)
		: _lock(lock)
		, _name(name)
	{
		_lock.WriteLock(_name);
	}

	~WriteLockGuard()
	{
		_lock.WriteUnlock(_name);
	}

private:
	Lock& _lock;
	const char* _name;
};