#pragma once
#include <mutex>
#include <condition_variable>

template<typename T>
class LockQueue
{
public:
	LockQueue() {};

	LockQueue(const LockQueue&) = delete;
	LockQueue& operator = (const LockQueue&) = delete;

	void Push(T value)
	{
		lock_guard<std::mutex> lock(_mutex);
		_queue.push(std::move(value));
		_condVar.notify_one();
	}

	// empty -> top -> pop ÇÑ¹ø¿¡
	bool TryPop(T& value)
	{
		lock_guard<std::mutex> lock(_mutex);
		if (_queue.empty())
		{
			return false;
		}

		value = std::move(_queue.front());
		_queue.pop();
		return true;
	}

	bool Empty()
	{
		lock_guard<std::mutex> lock(_mutex);
		return _queue.empty();
	}

	void WaitPop(T& value)
	{
		unique_lock<std::mutex> lock(_mutex);
		_condVar.wait(lock, []() { return _queue.empty() == false; });
		value = std::move(_queue.front());
		_queue.pop();
	}

private:
	queue<T> _queue;
	std::mutex _mutex;
	condition_variable _condVar;
};