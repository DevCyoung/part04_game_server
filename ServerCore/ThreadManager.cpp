#include "pch.h"
#include "ThreadManager.h"

ThreadManager::ThreadManager()
{
	InitTLS();
}

ThreadManager::~ThreadManager()
{
	Join();
}

void ThreadManager::Launch(function<void(void)> callback)
{
	LockGuard guard(_lock);

	_threads.push_back(std::thread([=]() {
		InitTLS();
		callback();
		DestroyTLS();
		}));
}

void ThreadManager::Join()
{
	for (thread& t : _threads)
	{
		if (t.joinable())
		{
			t.join();
		}
	}
}

void ThreadManager::InitTLS()
{
	static Atomic<uint32> SThreadID = 0;
	LThreadId = SThreadID.fetch_add(1);
}

void ThreadManager::DestroyTLS()
{
}
