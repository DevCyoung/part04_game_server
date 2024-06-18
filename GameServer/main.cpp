#include "pch.h"
#include <thread>
#include <atomic>
#include <mutex>
#include <Windows.h>
#include <assert.h>
#include <future>

int64 result = 0;

void PromiseWorker(std::promise<string>&& promise)
{
	promise.set_value("sevret message");
}

void TeaskWorker(std::packaged_task<int64(void)>&& task)
{
	task();
}

//���������Ѱ��
int64 Calculate()
{
	int sum = 0;
	for (int32 i = 0; i < 1000'000; i++)
	{
		sum += i;
	}

	result = sum;

	return sum;
}

int main()
{
	// ����( synchronous) ����
	//int64 sum = Calculate();

	//���� Caculate�� ���� �����ɸ��� �Լ����?
	//�񵿱����� ����������!

	//�������
	//thread t(Calculate);
	//t.join();

	//std::future ��뿹��
	{
		//1) deferred -> lazy evaluation �����ؼ� �����ϼ��� (�׳ɵ���) Ŀ�ǵ������� �������� ���� ��������� �ڷιи� ��Ժ��� �񵿱��� ����
		//2) async -> ������ �����带 ���� �����ϼ��� (�츮�� ������ �����¹��) (�����带 ���ķ� ��������)
		//3) deferred | async -> �� �� �˾Ƽ� ����ּ���
		std::future<int64> future = std::async(std::launch::async, Calculate); //������ �̷��� ������� ����ٰž�
		//get�� �ϴ¼��� ������� ���ü��ִٰ� ���
		//�����带 ���� �����ϰ� ����Ҽ��� ������ �ѹ�������Ҷ� �����ϴ� (���ϰ� ����Ҽ�����)
		//�񵿱��̱� ������ ������ ��������		
		//TODO
		//�����Ŀ� ������ ��������? ���
		//std::future_status status =  future.wait_for(1ms);

		//future.wait(); get �̳� wait�� �Ȱ���

		int64 sum = future.get(); // ������� �������� �ʿ��ϴ�!


		//����Լ��� ȣ���ϰ� ������?

		class Knight
		{
		public :
			int64 GetHP() { return 100; }
		};

		Knight knight;
		std::future<int64> future2 = std::async(std::launch::async, &Knight::GetHP, knight); //knight.GetHP()
	}


	//std::promise future��ü�� ����� �ι�°���
	{
		//�̷���(std::futrue)�� ������� ��ȯ���ٰŶ� ��� ����~ ��༭?
		//�������ϰ� ���� ������ �̿��ϱ⺸�ٴ� promise�� ¦�����缭 ���
		std::promise<string> promise; //pending
		std::future<string> future = promise.get_future(); //pending

		thread t(PromiseWorker, std::move(promise)); //empty ���̻� �ǵ帮��ȵ�

		string message = future.get();//empty ���̻� �ǵ帮��ȵ�
		//future.get(); 

		t.join();
	}


	//std::packaged_task
	{
						//��ȯŸ��,��ǲŸ�Ը������
		std::packaged_task<int64(void)> task(Calculate);
		std::future<int64> future = task.get_future();

		//������� future�� ���� �˼�����
		std::thread t(TeaskWorker, std::move(task));

		int64 sum = future.get();

		t.join();
	}
}