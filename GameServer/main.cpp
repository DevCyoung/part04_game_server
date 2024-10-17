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

		//시그널상태로 바꿔주세요
		::SetEvent(handle);
		std::this_thread::sleep_for(1ms);
	}
}

void Consumer()
{
	while (true)
	{
		//잠든다. handle이 파란불이되면 빠져나옴
		::WaitForSingleObject(handle, INFINITE);
		::ResetEvent(handle);
		//깨어난후에 자동으로(오토리셋)
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
	//첫번째는 보안속성 일단 NULL
	//두번쨰는 수동리셋? 자동리셋?
	//메뉴얼 = 수동 

	//이벤트는 커널에서 만들어주는 커널 오브젝트
	// Usage Count
	// Signal (파란불 TRUE bInitialState) / Non-Signal (빨간불 FALSE bInitialState) << 내부적으론 bool 모든 커널오브젝트의 공통된 속성
	// Auto / Manual < bool 상대적으로 가벼운 Event 커널 오브젝트
	handle = ::CreateEvent(NULL/* 보안속성 */, FALSE /*bManualReset*/, FALSE /*bInitialState*/, NULL);
	
	std::thread t1(Producer);
	std::thread t2(Consumer);

	t1.join();
	t2.join();

	//handle 종료
	::CloseHandle(handle);

}
