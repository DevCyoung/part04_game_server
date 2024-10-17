#include "pch.h"
#include <mutex>
#include <atomic>
#include <Windows.h>
#include <future>

int64 Caculate()
{
	int64 sum = 0;

	for (int32 i = 0; i < 100'000; ++i)
	{
		sum += i;
	}

	return sum;
}

void PromiseWorker(std::promise<string>&& promise)
{
	while (true);
	promise.set_value("Scret Message");

}

void TaskWorker(std::packaged_task<int64(void)>&& task)
{
	task();
}

int main()
{
	//���� ����� ���� synchronous
	//int64 sum = Caculate();

	//async�� �ɼ�
	//deferred -> lazy evaluation �����ؼ� �����ϼ���		�ͺ�ŷ(������ ��������)
	//async -> ������ �����带 ���� �����ϼ���			�ͺ�ŷ(������ ��������)
	//deferred | async -> �� �� �˾Ƽ� �� ����ּ���
	//Ŀ�ǵ������� �������� ����

	//�ϴ� ���������°� �ͺ�ŷ
	std::future<int64> future = std::async(std::launch::async, Caculate);
	
	//��������� �ڷιи��� = �񵿱�

	//TODO

	//������ �̷��� ������� ����ٲ���
	std::future_status status =  future.wait_for(0ms);
	if (status == future_status::ready)
	{
	}

	int64 sum = future.get(); //�������� �ʿ��ϴ�!

	//����Լ��� ȣ���ϰ������

	class Knight
	{
	public:
		int64 GetHP() {
			return 100;
		}
	};

	Knight knight;
	std::future<int64> future2 = std::async(std::launch::async, &Knight::GetHP, knight);

	{
		// �̷�(std::future)�� ������� ��ȯ���ٲ��� �������(std::promise)
		// �������ϰ� ���������� �̿������ʾƵ��ȴ�.
		std::promise<std::string> promise;
		std::future<string> future = promise.get_future();

		//promise�� PromiseWorker�� �����ϰԵ�
		//ǻó�� �ݵ�� ������ΰ��� �ƴ�
		thread t(PromiseWorker, std::move(promise));

		std::string message = future.get();
		future.get();
		std::cout << message << std::endl;

		t.join();
	}

	// stdLLpackaged_task
	{
		std::packaged_task<int64(void)> task(Caculate);
		std::future<int64> future = task.get_future();

		std::thread t(TaskWorker, std::move(task));

		future.get();

		t.join();
	}

	// ���)
	// mutex, condition_variable ���� �����ʰ� �ܼ��� �ֵ��� ó���� �� �ִ�
}
