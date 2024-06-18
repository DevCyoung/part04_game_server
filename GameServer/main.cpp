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

//뭔가복잡한계산
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
	// 동기( synchronous) 실행
	//int64 sum = Calculate();

	//만약 Caculate가 정말 오래걸리는 함수라면?
	//비동기방식이 유용해진다!

	//기존방법
	//thread t(Calculate);
	//t.join();

	//std::future 사용예제
	{
		//1) deferred -> lazy evaluation 지연해서 실행하세요 (그냥동기) 커맨드패턴의 전형적인 패턴 실행시점이 뒤로밀림 어떻게보면 비동기임 ㅋㅋ
		//2) async -> 별도의 쓰레드를 만들어서 실행하세요 (우리가 관심을 가지는방법) (스레드를 병렬로 실행해줌)
		//3) deferred | async -> 둘 중 알아서 골라주세요
		std::future<int64> future = std::async(std::launch::async, Calculate); //언젠가 미래에 결과물을 뱉어줄거야
		//get을 하는순간 결과물을 얻어올수있다고 약속
		//스레드를 만들어서 관리하고 사용할수도 있지만 한번만사용할때 용이하다 (편하게 사용할수있음)
		//비동기이기 때문에 무조건 리턴을함		
		//TODO
		//한참후에 그일이 끝났을까? 사용
		//std::future_status status =  future.wait_for(1ms);

		//future.wait(); get 이나 wait나 똑같음

		int64 sum = future.get(); // 결과물이 이제서야 필요하다!


		//멤버함수를 호출하고 싶을때?

		class Knight
		{
		public :
			int64 GetHP() { return 100; }
		};

		Knight knight;
		std::future<int64> future2 = std::async(std::launch::async, &Knight::GetHP, knight); //knight.GetHP()
	}


	//std::promise future객체를 만드는 두번째방법
	{
		//미래에(std::futrue)에 결과물을 반환해줄거라 약속 해줘~ 계약서?
		//지저분하게 전역 변수를 이용하기보다는 promise로 짝을맞춰서 사용
		std::promise<string> promise; //pending
		std::future<string> future = promise.get_future(); //pending

		thread t(PromiseWorker, std::move(promise)); //empty 더이상 건드리면안됨

		string message = future.get();//empty 더이상 건드리면안됨
		//future.get(); 

		t.join();
	}


	//std::packaged_task
	{
						//반환타입,인풋타입맞춰야함
		std::packaged_task<int64(void)> task(Calculate);
		std::future<int64> future = task.get_future();

		//결과물이 future을 통해 알수있음
		std::thread t(TeaskWorker, std::move(task));

		int64 sum = future.get();

		t.join();
	}
}