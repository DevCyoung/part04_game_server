#include "pch.h"
#include <thread>
#include <atomic>
#include <mutex>
#include <Windows.h>
#include <assert.h>
#include <future>

#include "ConcurrentStack.h"
#include "ConcurrentQueue.h"

LockFreeStack<int32> lockStack;
LockQueue<int32> lockQueue;

void Push()
{
	while (1)
	{
		lockStack.Push(rand() % 1000);
	}
}

void Pop()
{
	while (1)
	{
		int32 value;
		lockStack.TryPop(value);
	}
}

int main()
{
	std::thread t1(Push);
	std::thread t2(Push);
	std::thread t3(Push);
	std::thread t4(Push);
	std::thread t5(Push);
	std::thread t6(Pop);
	std::thread t7(Pop);

	t1.join();
	t2.join();
}
