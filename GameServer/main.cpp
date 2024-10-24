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
CoreGlobal GCore;

void ThreadMain()
{
	while (true)
	{
		std::cout << "Hello ! I am Thread..." << LThreadId << endl;
		this_thread::sleep_for(1s);
	}
}

int main()
{
	for (int32 i = 0; i < 5; ++i)
	{
		GThreadManager->Launch(ThreadMain);		
	}

	GThreadManager->Join();
}