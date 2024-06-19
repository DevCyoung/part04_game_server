#include "pch.h"
#include <thread>
#include <atomic>
#include <mutex>
#include <Windows.h>
#include <assert.h>
#include <future>

//가시성 (코드재배치 컴파일러입장에서) 컴파일러가 하지않더라도 CPU가 뒤집일수도있다 (디스어셈블리가 그렇다하더라도 CPU가 멋대로 바꾼다)
static int32 x = 0;
static int32 y = 0;
static int32 r1 = 0;
static int32 r2 = 0;

static volatile bool flag = false;

void Thread_1()
{
	while (!flag)
	{
	}
	y = 1; // Store y 여기서  y = 1을 대입한 명령어가 있다고해서 곧이 곧대로 처박는게아님 (캐시에 넣을수도있음) 이를 가시성 이라고한다.
	r1 = x; // Load x
}

void Thread_2()
{
	while (!flag)
	{
	}

	x = 1; // Store x
	r2 = y; // Load y 여기서 로드하는게 진짜 메모리에서 로드하는게 아닐수도있음 (캐시에서로드함)
}

int main()
{
	int32 count = 0;

	while (true)
	{
		flag = false;

		++count;

		x = y = r1 = r2 = 0;

		thread t1(Thread_1);
		thread t2(Thread_2);

		flag = true;

		t1.join();
		t2.join();

		if (r1 == 0 && r2 == 0)
		{
			break;
		}
	}

	cout << count << endl;
}
