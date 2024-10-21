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

template<typename T>
class LockFreeStack
{
	struct Node
	{
		Node(const T& value)
			: data(value)
		{
		}

		T data;
		Node* next;
	};

public:

	void Push(const T& value)
	{
		Node* node = new Node(value);
		node->next = _head;

		while (false == _head.compare_exchange_weak(node->next, node))
		{
		}

		/*if (_head == node->next)
		{
			node->next = _head;
			_head = node;
			return true;
		}
		else
		{
			node->next = _head;
			return false;
		}*/
	}

	bool TryPop(T& value)
	{
		//if (nullptr == _head)
		//{
		//	return false;
		//}

		/*if (_head == oldHead)
		{
			oldHead = _head;
			_head = _head->next;
			return true;
		}
		else
		{
			oldHead = _head;
			return false;
		}*/

		Node* oldHead = _head;
		while (oldHead && false == _head.compare_exchange_weak(oldHead, oldHead->next))
		{
		}

		if (nullptr == oldHead)
		{
			return false;
		}


		value = oldHead->data;
		delete oldHead;
		return true;
	}

private:
	atomic<Node*> _head;
};