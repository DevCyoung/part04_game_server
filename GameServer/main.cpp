#include "pch.h"
#include <thread>
#include <atomic>
#include <mutex>
#include <Windows.h>
#include <assert.h>

//이벤트라는것의 응용

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


		::SetEvent(handle); //커널오브젝트를 시그널상태로 바꿔주세요

		this_thread::sleep_for(1000000ms);
	}
}

void Consumer()
{
	while (true)
	{
		::WaitForSingleObject(handle, INFINITE); //커널오브젝트의 시그널상태를봄. 시그널상태면 넘어감 넌시그널이면 잠듬 Ready
		//Auto-Reset을 켜놨으면 이스레드를 깨우면서 Non-Signal로 만든다.
		//메뉴얼 설정을 TRUE로 해놨으면
		//을 호출해서 ::ResetEvent(handle); 시그널을 꺼야한다.

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
	//FALSE 여야 오토리셋이다
	//FALSE 로둬서 시그널을끈다.
	//커널에서 만들어주는 커널오브젝트
	//Usage Count
	//Singnal(파란불), Non-Signal(빨간불) (커널오브젝트가 가진 공통적인속성)	
	handle =  ::CreateEvent(NULL/*보안속성*/, FALSE, FALSE, NULL);


	thread t1(Producer);
	thread t2(Consumer);

	t1.join();
	t2.join();

	assert(handle != 0);
	::CloseHandle(handle);
}