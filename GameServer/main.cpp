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

		/*if (_locked == expected)
		{
			expected = _locked;
			_locked = desired;
			return true;
		}
		else
		{
			expected = _locked;
			return false;
		}*/



		/*while (false == _locked.compare_exchange_strong(expected, desired))
		{

		}*/

		while (false == _locked.compare_exchange_strong(expected, true))
		{
			expected = false;
		}
	}

	void unlock()
	{
		_locked.store(false);
	}

private:
	//c++ 에서의 volatile은 C# Java에서의 volatile와 다름
	//컴파일러에게 최적화하지 말아달라고 하는것에 불과함
	//volatile bool _locked = false;

	//volatile을 포함하는 atomic
	//원자적으로 동작함
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