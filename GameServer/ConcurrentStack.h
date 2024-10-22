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

	// empty -> top -> pop �ѹ���
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
			, next(nullptr)

		{

		}

		T data;
		Node* next;
	};

public:

	LockFreeStack()
		: _head(nullptr)
		, _pendingList(nullptr)
		, _popCount(0)
	{

	}

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

		++_popCount;

		Node* oldHead = _head;
		while (oldHead && false == _head.compare_exchange_weak(oldHead, oldHead->next))
		{
		}

		if (nullptr == oldHead)
		{
			--_popCount;
			return false;
		}

		//����� ������ �Ѿ��
		value = oldHead->data;

		TryDelete(oldHead);
		//delete oldHead;
		return true;
	}

	// �����ͺи�
	// Count üũ
	// �� ȥ�ڸ� ����
	void TryDelete(Node* oldHead)
	{
		//�� �ܿ� ���� �ִ°�?
		if (_popCount == 1)
		{
			// �� ȥ�ڳ�?

			// �̿� ȥ���ΰ�, ���� ����� �ٸ� �����͵鵵 �����غ���
			Node* node = _pendingList.exchange(nullptr);

			//���鼭 ������� ����ش�
			if (--_popCount == 0)
			{
				// ����� �ְ� ���� -> ���� ����
				// �����ͼ� �����, ������ �����ʹ� �и��ص� ����~!
				DeleteNodes(node);
			}
			else if (node)
			{
				// ���� ���������ϴٽ� ���� ����
				ChainPendingNodeList(node);
			}

			// oldHead ������ ����! �� �����ʹ� ����
			delete oldHead;
		}
		else
		{
			// ���� �ֳ�? �׷� ���ݻ��������ʰ� ����
			ChainPendingNode(oldHead);
			--_popCount;
		}
	}
					//last		// pending list
	//[] [] [] [] []   [] -> [] [] [] [] []
	void ChainPendingNodeList(Node* first, Node* last)
	{
		last->next = _pendingList;

		while (false == _pendingList.compare_exchange_weak(last->next, first))
		{
		}
	}

	void ChainPendingNodeList(Node* node)
	{
		Node* last = node;

		while (last->next)
		{
			last = last->next;
		}

		ChainPendingNodeList(node, last);
	}

	void ChainPendingNode(Node* node)
	{
		ChainPendingNodeList(node, node);
	}

	static void DeleteNodes(Node* node)
	{
		while (node)
		{
			Node* next = node->next;
			delete node;
			node = next;
		}
	}

private:
	atomic<Node*> _head;
	atomic<Node*> _pendingList; // ���� �Ǿ�� �� ����
	atomic<uint32> _popCount; // Pop�� �������� ������ ����
};