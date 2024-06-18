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

//����) CV�� User-LEvel Object (Ŀ�ο�����ƮX)
condition_variable cv; //�̰Ž�ǥ�� ����������ص��ȴ�. ��õ�Ǵ� ���������

void Producer()
{
	//1 Lock�� ��´�.
	//2 ���� ���� ���� ����
	//3 Lock�� Ǯ��
	//4 ���Ǻ������� �ٸ� �����忡�� ����
	while (true)
	{
		{
			unique_lock<mutex> lcok(m);
			q.push(100);
		}		


		cv.notify_one(); //waitw���� �����尡�ִٸ� �� 1���� �ƿ��.
	}
}

void Consumer()
{
	while (true)
	{		
		unique_lock<mutex> lock(m);
		cv.wait(lock, []() {return q.empty() == false; });
		// 1 Lock����� (������ �̹��������� ����������� ���������� ��������õ�)
		// 2 ����Ȯ��
		// 3 �����ϸ� ���������ͼ� �̾ �ڵ带 ����
		// 4 ���������ʴ´� (Lock�� Ǯ���ְ� �����·� �����) ������δ¾ȵ� ���ж����θ� ����ؾ���
		// ������ �������������� ����Ǯ�� wait���Ѵ�.
		// wait���λ��¿��� cv.notify_one() ���ϰ� ������� ������Ȯ���̰ɴٽ�����

		// �׷��� Notyfi_one�� ������ �׻� ���ǽ��� �����ϴ°� �ƴұ�?
		// Spurius Wakeup ��¥���??
		// notify_one�� �Ҷ� lock�� ��� �ִ°��� �ƴϱ⶧����

		int32 data = q.front();
		q.pop();
		cout << data << endl;

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