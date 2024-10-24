#pragma once

#include <thread>
#include <functional>

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

	void Launch(function<void(void)> callback);
	void Join();

	static void InitTLS();
	static void DestroyTLS();

private:
	Mutex	_lock;
	vector<thread> _threads;
};
