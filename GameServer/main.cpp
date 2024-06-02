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
	//c++ ������ volatile�� C# Java������ volatile�� �ٸ�
	//�����Ϸ����� ����ȭ���� ���ƴ޶�� �ϴ°Ϳ� �Ұ���
	//volatile bool _locked = false;

	//volatile�� �����ϴ� atomic
	//���������� ������
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