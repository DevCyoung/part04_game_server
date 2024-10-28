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

#include "PlayerManager.h"
#include "AccountManager.h"

// 소수 구하기

// 1과 자기 자신으로만 나뉘면 그것을 소수라고함

int Zegob(int num)
{
	int i = 1;

	for (i = 1; i * i < num; ++i)
	{		
	}

	return i;
}

bool IsPrime(int num)
{
	if (num < 2)
	{
		return false;
	}

	if (num == 2 || num == 3)
	{
		return true;
	}

	int nex = Zegob(num);

	for (int i = 2; i <= nex; ++i)
	{
		if ((num % i) == 0)
		{
			return false;
		}
	}

	return true;
}

// 200 ~ 999
int PrimeCount(int start, int end)
{
	int count = 0;

	for (; start <= end; ++start)
	{
		if (IsPrime(start))
		{
			++count;
			//std::cout << start << std::endl;
		}
	}

	return count;
}

int main()
{
	const int MAX_NUMBER = 1000000;

	// 1~MAX_NUMBER까지의 소수 개수

	const int32 threadCount = 100;

	std::future<int> func[threadCount];

	// 1 ~ 100
	// 1, 0 ~ 100
	// 2, 
	//   1 ~ 100
	// 101 ~ 200
	// 201 ~ 300
	for (int i = 0; i < threadCount; ++i)
	{
		int start = i * (MAX_NUMBER / threadCount) + 1;
		int end = (i + 1) * (MAX_NUMBER / threadCount);

		//std::cout << i * 1000 + 1 << " " << (i + 1) * 1000 << std::endl;
		func[i] = std::async(PrimeCount, start, end);
	}

	int ret = 0;
	for (int i = 0; i < threadCount; ++i)
	{
		ret += func[i].get();
	}

	std::cout << ret << std::endl;
}