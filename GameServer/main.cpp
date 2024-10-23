#include "pch.h"
#include <thread>
#include <atomic>
#include <mutex>
#include <Windows.h>
#include <assert.h>
#include <future>
#include "ConcurrentStack.h"
#include "ConcurrentQueue.h"

LockQueue<int32> q;
LockFreeStack<int32> s;
std::atomic<int64> freelock;

void Push()
{
	freelock.fetch_add(1);


	while (true)
	{
		int32 value = rand() % 100;
		s.Push(value);		

		//this_thread::sleep_for(100ms);
	}

}

void Pop()
{
	while (true)
	{
		//if (q.empty())
		//{
		//	continue;
		//}
		//
		//int32 value = q.front();
		//q.pop();

		int32 value;
		s.TryPop(OUT value);
		std::cout << value << std::endl;
	}

	freelock.fetch_add(1);
}

int main()
{
	
	std::mutex mm;

	freelock.store(10);

	thread t1(Push);
	thread t2(Pop);
	freelock.store(11);
	t1.join();
	t2.join();
}