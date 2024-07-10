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

	// 1. ����带 �����
	// 2. �� ����� next = head
	// 3. head = �����
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

		// �� ���̿� ��ġ�� ���ϸ�?
		//head = node;
	}

	// 1. head �б�
	// 2. head->next �б�
	// 3. head = head->next
	// 4. data �����ؼ� ��ȯ
	// 5. ������ ��带 ����
	// ��ġ�� ���ɼ�����! CAS�� �ؾ���!
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
			//oldHead = _head //������ ������ �¾� ������!
		}

		if (nullptr == oldHead)
		{
			return false;
		}

		//Exception X �츮�� �Ű�Ⱦ�
		value = oldHead->data;
		//_head = _head.next;

		delete oldHead; //�����ȸ޸��ε� ũ���ð� ������������ �̰��� ó���ϱ����� ������ �پ��� ������ִ�.

		return true;
	}

private:
	//[ ] [ ] [ ] [ ] [ ]
	//[Head]

	atomic<Node*> _head;
};