#pragma once

#include <thread>
#include <functional>
#include <mutex>
#include <vector>
/*------------------------
	ThreadManager
------------------------*/

class ThreadManager
{
public:
	ThreadManager();
	virtual ~ThreadManager();

	ThreadManager(const ThreadManager&) = delete;
	ThreadManager& operator = (const ThreadManager&) = delete;

	void Launch(std::function<void(void)> callback);
	void Join();

	static void InitTLS();
	static void DestroyTLS();

private:
	std::mutex	_lock;
	std::vector<std::thread> _threads;
};
