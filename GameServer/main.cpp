#include "pch.h"
#include <thread>
#include <atomic>
#include <mutex>

volatile int32 sum = 0;
mutex m;

//int num = 100_000;
class SpinLock
{
public:
	void lock()
	{
		//CAS (Compare-And-Swap)
		bool expected = false;
		bool desired = true;

		while (false == _locked.compare_exchange_strong(expected, true))
		{			
			expected = false;


			//C++11표준의 Sleep
			//this_thread::sleep_for(std::chrono::milliseconds(100));
			//this_thread::sleep_for(0ms);

			this_thread::yield(); //== this_thread::sleep_for(0ms)//알아서 스케쥴링해라
		}
	}

	void unlock()
	{
		_locked.store(false);
	}

private:
	atomic<bool> _locked = false;
};

SpinLock sp;

void Add()
{
	for (int32 i = 0; i < 10'0000; ++i)
	{
		lock_guard<SpinLock> lock(sp);
		sum++;
	}		
}

void Sub()
{
	for (int32 i = 0; i < 10'0000; ++i)
	{
		lock_guard<SpinLock> lock(sp);
		sum--;
	}
}

int main()
{
	int32 a = 0;
	a = 1;
	a = 2;
	a = 3;
	a = 4;

	thread t1(Add);
	thread t2(Sub);

	t1.join();
	t2.join();
	cout << sum << endl;
}