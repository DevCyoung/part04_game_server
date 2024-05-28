#include "pch.h"
#include <thread>

//ǥ��
#include <atomic>
//atomic atom(����)
//int sum = 0;
atomic<int32> sum = 0;

//DB
//// Atolic
// A��� ���� �κ����� ����˻���
// B��� ���� �κ����� ��������߰�
////

void Add()
{
	for (int32 i = 0; i < 1'000'000; ++i)
	{
		//++sum;
		sum.fetch_add(1);
	}
}

void Sub()
{
	for (int32 i = 0; i < 1'000'000; ++i)
	{
		//--sum;
		sum.fetch_add(-1);
	}
}

int main()
{	
	std::cout << sum++ << std::endl;
	Add();
	Sub();
	cout << sum << endl;

	std::thread t1(Add);
	std::thread t2(Sub);

	t1.join();
	t2.join();

	cout << sum << endl;
}