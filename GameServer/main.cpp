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

		//�ñ׳λ��·� �ٲ��ּ���
		::SetEvent(handle);
		std::this_thread::sleep_for(1ms);
	}
}

void Consumer()
{
	while (true)
	{
		//����. handle�� �Ķ����̵Ǹ� ��������
		::WaitForSingleObject(handle, INFINITE);
		::ResetEvent(handle);
		//����Ŀ� �ڵ�����(���丮��)
		//-> Non-Signal

		std::unique_lock<std::mutex> lock(m);
		if (false == q.empty())
		{
			int32 data = q.front();
			q.pop();
			std::cout << data << std::endl;
		}
	}
}

void State()
{
}

int main()
{
	//ù��°�� ���ȼӼ� �ϴ� NULL
	//�ι����� ��������? �ڵ�����?
	//�޴��� = ���� 

	//�̺�Ʈ�� Ŀ�ο��� ������ִ� Ŀ�� ������Ʈ
	// Usage Count
	// Signal (�Ķ��� TRUE bInitialState) / Non-Signal (������ FALSE bInitialState) << ���������� bool ��� Ŀ�ο�����Ʈ�� ����� �Ӽ�
	// Auto / Manual < bool ��������� ������ Event Ŀ�� ������Ʈ
	handle = ::CreateEvent(NULL/* ���ȼӼ� */, FALSE /*bManualReset*/, FALSE /*bInitialState*/, NULL);
	
	std::thread t1(Producer);
	std::thread t2(Consumer);

	t1.join();
	t2.join();

	//handle ����
	::CloseHandle(handle);

}
