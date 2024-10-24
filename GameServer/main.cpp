#include "pch.h"
#include <thread>
#include <atomic>
#include <mutex>
#include <Windows.h>
#include <assert.h>
#include <future>
#include "ConcurrentStack.h"
#include "ConcurrentQueue.h"
#include "ThreadManager.h"
#include "Lock.h"

CoreGlobal GCore;

class TestLock
{
	USE_LOCK;

public:
	int32 TestRead()
	{
		READ_LOCK;
		
		if (_queue.empty()) 
		{
			return -1;
		}

		return _queue.front();
	}

	void TestPush()
	{
		WRITE_LOCK;

		_queue.push(rand() % 1000);
	}

	void TestPop()
	{
		WRITE_LOCK;

		if (false == _queue.empty())
		{
			_queue.pop();
		}
	}

private:
	queue<int32> _queue;

};

TestLock testLock;


void ThreadWrite()
{
	while (true)
	{
		testLock.TestPush();
		this_thread::sleep_for(1ms);
		testLock.TestPop();
	}
}

void ThreadRead()
{
	while (true)
	{
		int32 value = testLock.TestRead();
		std::cout << value << std::endl;
		std::this_thread::sleep_for(1ms);
	}
}

int main()
{
	for (int32 i = 0; i < 2; ++i)
	{
		GThreadManager->Launch(ThreadWrite);
	}

	for (int32 i = 0; i < 10; ++i)
	{
		GThreadManager->Launch(ThreadRead);
	}

	GThreadManager->Join();
}