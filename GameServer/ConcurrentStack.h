#pragma once

#include <mutex>

template<typename T>
class LockStack
{
public:
	LockStack() {};
	LockStack(const LockStack&) = delete;
	LockStack& operator = (const LockStack& other) = delete;

	void Push(T value)
	{
		lock_guard<mutex> lock(_mutex);
		_stack.push(std::move(value));

		_conVar.notify_one();
	}

	bool TryPop(T& value)
	{
		lock_guard<mutex> lock(_mutex);
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
		lock_guard<mutex> lock(_mutex);
		return _stack.empty();
	}

	void WaitPop(T& value)
	{
		unique_lock<mutex> lock(_mutex);
		_conVar.wait(lock, [this] { return this->_stack.empty() == false; });
		value = std::move(_stack.top());
		_stack.pop();
	}

private:
	stack<T> _stack;
	mutex _mutex;
	condition_variable _conVar;
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

	// 1. 새노드를 만들고
	// 2. 새 노드의 next = head
	// 3. head = 새노드
	void Push(const T& value)
	{
		Node* node = new Node(value);
		node->next = _head;

		/*if (_head == node->next)
		{
			_head = node;
			return true;
		}
		else
		{
			node->next = _head;
			return false;
		}*/

		while (_head.compare_exchange_weak(node->next, node) == false)
		{
		}

		// 이 사이에 새치기 당하면?
		//head = node;
	}

	// 1. head 읽기
	// 2. head->next 읽기
	// 3. head = head->next
	// 4. data 추출해서 반환
	// 5. 추출한 노드를 삭제
	// 새치기 가능성있음! CAS로 해야함!
	bool TryPop(T& value)
	{
		Node* oldHead = _head;

		//if (_head == oldHead)
		//{
		//	_head = oldHead->next;
		//	return true;
		//}
		//else
		//{
		//	oldHead = _head;
		//	return false;
		//}

		while (oldHead && _head.compare_exchange_weak(oldHead, oldHead->next) == false)
		{
			//oldHead = _head //운좋게 조건이 맞아 떨어짐!
		}

		if (nullptr == oldHead)
		{
			return false;
		}

		//Exception X 우리는 신경안씀
		value = oldHead->data;
		//_head = _head.next;

		delete oldHead; //오염된메모리인데 크래시가 나버릴수있음 이것을 처리하기위해 굉장히 다양한 방법이있다.

		return true;
	}

private:
	//[ ] [ ] [ ] [ ] [ ]
	//[Head]

	atomic<Node*> _head;
};