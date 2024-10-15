#include "pch.h"
#include <thread>
#include <mutex>

#include "AccountManager.h"
#include "UserManager.h"

void Func1()
{
	for (int32 i = 0; i < 100; ++i)
	{
		UserManager::Instance()->ProcessSave();
	}
}

void Func2()
{
	for (int32 i = 0; i < 100; ++i)
	{
		AccountManager::Instance()->ProcessLogin();
	}
}
#include <Windows.h>

#ifdef _WIN64
#define XM_PI 1
#else
#define XM_PI 2
#endif

#ifdef _DEBUG
#endif
#include <assert.h>
int main()
{	
	std::thread t1(Func1);
	std::thread t2(Func2);
	//assert()
	t1.join();
	t2.join();

	std::cout << "Jobs Done" << std::endl;

	using iiids32 = int32;

	iiids32 ppap = 111;
	//DWORD_PTR

}