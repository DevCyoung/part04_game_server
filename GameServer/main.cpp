#include "pch.h"

//C++11 ���� ����������� ǥ������ ����
//�ް��ϸ� ����ѹ������ ����
#include <thread>
//nc�¾����� ��������
//�ؿܴ� ������

void HelloThread()
{
	cout << "Hello Thread" << endl;
}

void HelloTrhead_2(int32 num)
{
	cout << num << endl;
}

int main()
{	
	vector<std::thread> v;
	for (int32 i = 0; i < 10; ++i)
	{
		v.push_back(std::thread(HelloTrhead_2, i));
	}

	for (int32 i = 0; i < 10; ++i)
	{
		if (v[i].joinable())
		{
			v[i].join();
		}
	}

	/** 
	* output
	01
	3
	5
	7
	9
	2
	6

	4
	8
	**/
	return 0;
}