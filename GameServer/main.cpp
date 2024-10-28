#include "pch.h"
#include <thread>
#include <atomic>
#include <mutex>
#include <Windows.h>
#include <assert.h>
#include <future>
#include "ConcurrentStack.h"
#include "ConcurrentQueue.h"
#include "ThreadManager.h"
#include "Lock.h"

CoreGlobal GCore;

#include "PlayerManager.h"
#include "AccountManager.h"


int main()
{
	GThreadManager->Launch([=] {
		while (true)
		{
			std::cout << "PlayerThenAccount" << std::endl;
			GPlayerManager.PlayerThenAccount();
			//this_thread::sleep_for(100ms);
		}
		});

	GThreadManager->Launch([=] {
		while (true)
		{
			std::cout << "AccountThenPlayer" << std::endl;
			GAccountManager.AccountThenPlayer();
			//this_thread::sleep_for(100ms);
		}
		});
}