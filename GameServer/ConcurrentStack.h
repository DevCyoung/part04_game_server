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

	// empty -> top -> pop 한번에
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

		//노드의 소유권 넘어옴
		value = oldHead->data;

		TryDelete(oldHead);
		//delete oldHead;
		return true;
	}

	// 데이터분리
	// Count 체크
	// 나 혼자면 삭제
	void TryDelete(Node* oldHead)
	{
		//나 외에 누가 있는가?
		if (_popCount == 1)
		{
			// 나 혼자네?

			// 이왕 혼자인거, 삭제 예약된 다른 데이터들도 삭제해보자
			Node* node = _pendingList.exchange(nullptr);

			//빼면서 결과값을 뱉어준다
			if (--_popCount == 0)
			{
				// 끼어든 애가 없음 -> 삭제 진행
				// 이제와서 끼어들어도, 어차피 데이터는 분리해둔 상태~!
				DeleteNodes(node);
			}
			else if (node)
			{
				// 누가 끼어들었으니다시 갖다 놓자
				ChainPendingNodeList(node);
			}

			// oldHead 삭제가 가능! 내 데이터는 삭제
			delete oldHead;
		}
		else
		{
			// 누가 있네? 그럼 지금삭제하지않고 예약
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
	atomic<Node*> _pendingList; // 삭제 되어야 할 노드들
	atomic<uint32> _popCount; // Pop을 실행중인 쓰레드 개수
};