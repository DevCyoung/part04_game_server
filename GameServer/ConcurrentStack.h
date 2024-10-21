#pragma once
#include <mutex>

template<typename T>
class LockStack
{
public:
	LockStack() {}

	LockStack(const LockStack&) = delete;
	LockStack& operator = (const LockStack*) = delete;

	void Push(T value)
	{
		lock_guard<std::mutex> lock(_mutex);
		_stack.push(std::move(value));

		_condVar.notify_one();
	}

	// empty -> top -> pop ÇÑ¹ø¿¡
	bool TryPop(T& value)
	{
		lock_guard<std::mutex> lock(_mutex);
		if (_stack.empty())
		{
			return false;
		}

		value = std::move(_stack.top());
		_stack.pop();
		return true;
	}

	bool Empty()
	{
		lock_guard<std::mutex> lock(_mutex);
		return _stack.emplace();
	}

	void WaitPop(T& value)
	{
		unique_lock<std::mutex> lock(_mutex);
		_condVar.wait(lock, [](){ return _stack.empty() == false; });
		value = std::move(_stack.top());
		_stack.pop();
	}

private:
	std::stack<T> _stack;
	std::mutex _mutex;
	condition_variable _condVar;
};