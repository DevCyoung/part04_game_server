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
	//동기 방식의 실행 synchronous
	//int64 sum = Caculate();

	//async의 옵션
	//deferred -> lazy evaluation 지연해서 실행하세요		넌블러킹(무조건 빠져나옴)
	//async -> 별도의 쓰레드를 만들어서 실행하세요			넌블러킹(무조건 빠져나옴)
	//deferred | async -> 둘 중 알아서 잘 골라주세요
	//커맨드패턴의 전형적인 패턴

	//일단 빠져나오는건 넌블러킹
	std::future<int64> future = std::async(std::launch::async, Caculate);
	
	//실행시점이 뒤로밀린다 = 비동기

	//TODO

	//언젠가 미래에 결과물을 뱉어줄꺼야
	std::future_status status =  future.wait_for(0ms);
	if (status == future_status::ready)
	{
	}

	int64 sum = future.get(); //이제서야 필요하다!

	//멤버함수를 호출하고싶을때

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
		// 미래(std::future)에 결과물을 반환해줄꺼라 약속해줘(std::promise)
		// 지저분하게 전역변수를 이용하지않아도된다.
		std::promise<std::string> promise;
		std::future<string> future = promise.get_future();

		//promise를 PromiseWorker가 소유하게됨
		//퓨처가 반드시 실행용인것은 아님
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

	// 결론)
	// mutex, condition_variable 까지 가지않고 단순한 애들을 처리할 수 있는
}
