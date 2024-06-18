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

//참고) CV는 User-LEvel Object (커널오브젝트X)
condition_variable cv; //이거슨표준 마음껏사용해도된다. 추천되는 좋은방법임

void Producer()
{
	//1 Lock을 잡는다.
	//2 공유 변수 값을 수정
	//3 Lock을 풀고
	//4 조건변수통해 다른 쓰레드에게 통지
	while (true)
	{
		{
			unique_lock<mutex> lcok(m);
			q.push(100);
		}		


		cv.notify_one(); //waitw중인 쓰레드가있다면 딱 1개를 꺠운다.
	}
}

void Consumer()
{
	while (true)
	{		
		unique_lock<mutex> lock(m);
		cv.wait(lock, []() {return q.empty() == false; });
		// 1 Lock을잡고 (지금은 이미잡혀있음 잡혔으면통과 안잡혔으면 잡으려고시도)
		// 2 조건확인
		// 3 만족하면 빠져서나와서 이어서 코드를 진행
		// 4 만족하지않는다 (Lock을 풀어주고 대기상태로 만든다) 락가드로는안됨 유닉락으로만 사용해야함
		// 조건을 만족하지않으면 락을풀고 wait를한다.
		// wait중인상태에서 cv.notify_one() 빵하고 락을잡고 조건을확인이걸다시진행

		// 그런데 Notyfi_one을 했으면 항상 조건식을 만족하는건 아닐까?
		// Spurius Wakeup 가짜기상??
		// notify_one을 할때 lock을 잡고 있는것이 아니기때문에

		int32 data = q.front();
		q.pop();
		cout << data << endl;

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