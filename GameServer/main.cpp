#include "pch.h"
#include <thread>
#include <atomic>
#include <mutex>
#include <Windows.h>
#include <assert.h>
#include <future>

//���ü� (�ڵ����ġ �����Ϸ����忡��) �����Ϸ��� �����ʴ��� CPU�� �����ϼ����ִ� (�𽺾������ �׷����ϴ��� CPU�� �ڴ�� �ٲ۴�)
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
	y = 1; // Store y ���⼭  y = 1�� ������ ��ɾ �ִٰ��ؼ� ���� ���� ó�ڴ°Ծƴ� (ĳ�ÿ� ������������) �̸� ���ü� �̶���Ѵ�.
	r1 = x; // Load x
}

void Thread_2()
{
	while (!flag)
	{
	}

	x = 1; // Store x
	r2 = y; // Load y ���⼭ �ε��ϴ°� ��¥ �޸𸮿��� �ε��ϴ°� �ƴҼ������� (ĳ�ÿ����ε���)
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
