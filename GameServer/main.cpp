#include "pch.h"
#include <mutex>
#include <atomic>
class SpinLock
{
public:
	void lock()
	{
		bool expected = false;
		bool desired = true;

		while (false == _locked.compare_exchange_strong(expected, desired))
		{
			expected = false;
		}
	}

	void unlock()
	{
		_locked.store(false);
	}

private:
	atomic<bool> _locked = false;
};

SpinLock spinLock;

std::mutex m;
int32 sum;

void Add()
{
	for (int32 i = 0; i < 100'000; ++i)
	{
		lock_guard<SpinLock> guard(spinLock);
		++sum;
	}
}

void Sub()
{
	for (int32 i = 0; i < 100'000; ++i)
	{
		lock_guard<SpinLock> guard(spinLock);
		--sum;
	}
}

void Test(volatile bool* b)
{
	//std::this_thread::sleep_for(10s);
	*b = false;
}

int main()
{	
	std::thread t1(Add);
	std::thread t2(Sub);

	t1.join();
	t2.join();

	std::cout << sum << std::endl;
}