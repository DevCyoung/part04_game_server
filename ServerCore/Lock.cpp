#include "pch.h"
#include "Lock.h"
#include "CoreTLS.h"
#include "DeadLockProfiler.h"

void Lock::WriteLock(const char* name)
{
#if _DEBUG
	GDeadLockProfiler->PushLock(name);
#endif

	// ������ �����尡 �����ϰ� �ִٸ� �ְ� ����
	const uint32 lockThreadId = (_lockFag.load() & WRITE_THREAD_MASK) >> 16;
	if (LThreadId == lockThreadId)
	{
		++_writeCount; //���������
		return;
	}

	const int64 beginTick = ::GetTickCount64();

	// �ƹ��� ���� �� �����ϰ� ���� ���� ��, �����ؼ� �������� ��´�.
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

	//���׸� ������ѿ뵵 ReadLock�� ��Ǯ�������� WriteUnLock �Ұ���
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

	// ������ �����尡 �����ϰ� �ִٸ� ������ ����, WriteLock -> ReadLock ���
	const uint32 lockThreadId = (_lockFag.load() & WRITE_THREAD_MASK) >> 16;
	if (LThreadId == lockThreadId)
	{
		_lockFag.fetch_add(1); //�̱۽����� �ΰ͸���
		return;
	}

	// �ƹ��� �����ϰ� ���� ������ �����ؼ� ���� ī��Ʈ�� �ø���.
	const int64 beginTick = ::GetTickCount64();
	while (true)
	{
		for (uint32 spinCount = 0; spinCount < MAX_SPIN_COUNT; ++spinCount)
		{
			uint32 expected = (_lockFag.load() & READ_COUNT_MASK);

			//�����ߴٸ� �ΰ�������
			//�������� WriteLock�� �������ְų�
			//������ ���̷� �������� ��ġ�⸦�ؼ� ���� �����ߴ�.
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

	// ������
	if ((_lockFag.fetch_sub(1) & READ_COUNT_MASK) == 0)
	{
		CRASH("MULTIPLE_UNLOCK");
	}
}
