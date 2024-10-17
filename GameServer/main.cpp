#include "pch.h"
#include <mutex>
#include <atomic>
#include <Windows.h>

std::mutex m;
std::queue<int32> q;
HANDLE handle;

condition_variable cv;

void Producer()
{
	while (true)
	{
		{					
			std::unique_lock<std::mutex> lock(m);
			q.push(100);
		}

		cv.notify_one();
	}
}

void Consumer()
{
	while (true)
	{
		std::unique_lock<std::mutex> lock(m);
		cv.wait(lock, []() {return q.empty() == false; });

		int32 data = q.front();
		q.pop();
		std::cout << q.size() << std::endl;
	}
}

void State()
{
}

int main()
{
	std::thread t1(Producer);
	std::thread t2(Consumer);

	t1.join();
	t2.join();
}
