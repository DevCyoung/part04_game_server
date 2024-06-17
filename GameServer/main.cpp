#include "pch.h"
#include <thread>
#include <atomic>
#include <mutex>
#include <Windows.h>
#include <assert.h>

//�̺�Ʈ��°��� ����

mutex m;
queue<int32> q;
HANDLE handle;

void Producer()
{
	while (true)
	{
		{
			unique_lock<mutex> lcok(m);
			q.push(100);
		}		


		::SetEvent(handle); //Ŀ�ο�����Ʈ�� �ñ׳λ��·� �ٲ��ּ���

		this_thread::sleep_for(1000000ms);
	}
}

void Consumer()
{
	while (true)
	{
		::WaitForSingleObject(handle, INFINITE); //Ŀ�ο�����Ʈ�� �ñ׳λ��¸���. �ñ׳λ��¸� �Ѿ �ͽñ׳��̸� ��� Ready
		//Auto-Reset�� �ѳ����� �̽����带 ����鼭 Non-Signal�� �����.
		//�޴��� ������ TRUE�� �س�����
		//�� ȣ���ؼ� ::ResetEvent(handle); �ñ׳��� �����Ѵ�.

		unique_lock<mutex> lock(m);
		if (q.empty() == false)
		{
			int32 data = q.front();
			q.pop();
			cout << data << endl;
		}
	}
}

int main()
{
	//FALSE ���� ���丮���̴�
	//FALSE �εּ� �ñ׳�������.
	//Ŀ�ο��� ������ִ� Ŀ�ο�����Ʈ
	//Usage Count
	//Singnal(�Ķ���), Non-Signal(������) (Ŀ�ο�����Ʈ�� ���� �������μӼ�)	
	handle =  ::CreateEvent(NULL/*���ȼӼ�*/, FALSE, FALSE, NULL);


	thread t1(Producer);
	thread t2(Consumer);

	t1.join();
	t2.join();

	assert(handle != 0);
	::CloseHandle(handle);
}