#include "pch.h"
#include "Lock.h"
#include "CoreTLS.h"
#include "DeadLockProfiler.h"

void Lock::WriteLock(const char* name)
{
#if _DEBUG
	GDeadLockProfiler->PushLock(name);
#endif

	// 동일한 쓰레드가 소유하고 있다면 주고선 성공
	const uint32 lockThreadId = (_lockFag.load() & WRITE_THREAD_MASK) >> 16;
	if (LThreadId == lockThreadId)
	{
		++_writeCount; //나만사용함
		return;
	}

	const int64 beginTick = ::GetTickCount64();

	// 아무도 소유 및 공유하고 있지 않을 때, 경합해서 소유권을 얻는다.
	const uint32 desired = ((LThreadId << 16) & WRITE_THREAD_MASK);
	while (true)
	{
		for (uint32 spinCount = 0; spinCount < MAX_SPIN_COUNT; ++spinCount)
		{
			uint32 expected = EMPTY_FLAG;
			if (_lockFag.compare_exchange_strong(OUT expected, desired))
			{
				++_writeCount;
				return;
			}
		}

		if (::GetTickCount64() - beginTick >= ACQUIRE_TIMEOUT_TICK)
		{
			CRASH("LOCK_TIMEOUT");
		}

		this_thread::yield();
	}
}

void Lock::WriteUnlock(const char* name)
{

#if _DEBUG
	GDeadLockProfiler->PopLock(name);
#endif

	//버그를 잡기위한용도 ReadLock을 다풀기전에는 WriteUnLock 불가능
	if ((_lockFag.load() & READ_COUNT_MASK) != 0)
	{
		CRASH("INVALID_UNLOCK_ORDER");
	}

	const int32 lockCount = --_writeCount;
	if (lockCount == 0)
	{
		_lockFag.store(EMPTY_FLAG);
	}
}

void Lock::RaedLock(const char* name)
{

#if _DEBUG
	GDeadLockProfiler->PushLock(name);
#endif

	// 동일한 쓰레드가 소유하고 있다면 무조건 성공, WriteLock -> ReadLock 허용
	const uint32 lockThreadId = (_lockFag.load() & WRITE_THREAD_MASK) >> 16;
	if (LThreadId == lockThreadId)
	{
		_lockFag.fetch_add(1); //싱글스레드 인것마냥
		return;
	}

	// 아무도 소유하고 있지 않을때 경합해서 공유 카운트를 올린다.
	const int64 beginTick = ::GetTickCount64();
	while (true)
	{
		for (uint32 spinCount = 0; spinCount < MAX_SPIN_COUNT; ++spinCount)
		{
			uint32 expected = (_lockFag.load() & READ_COUNT_MASK);

			//실패했다면 두가지원인
			//누군가가 WriteLock을 가지고있거나
			//간발의 차이로 누군가가 새치기를해서 먼저 실행했다.
			if (_lockFag.compare_exchange_strong(OUT expected, expected + 1))
			{
				return;
			}
		}

		if (::GetTickCount64() - beginTick >= ACQUIRE_TIMEOUT_TICK)
		{
			CRASH("LOCK_TIMEOUT");
		}

		this_thread::yield();
	}
}

void Lock::ReadUnlock(const char* name)
{
#if _DEBUG
	GDeadLockProfiler->PopLock(name);
#endif

	// 이전값
	if ((_lockFag.fetch_sub(1) & READ_COUNT_MASK) == 0)
	{
		CRASH("MULTIPLE_UNLOCK");
	}
}
