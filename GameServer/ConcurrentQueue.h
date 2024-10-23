#pragma once

#include <mutex>

template<typename T>
class LockQueue
{
public:
	LockQueue() {};
	LockQueue(const LockQueue&) = delete;
	LockQueue& operator = (const LockQueue&) = delete;

	void Push(T value)
	{
		lock_guard<mutex> lock(_mutex);
		_queue.push(std::move(value));

		_conVar.notify_one();
	}

	void TryPop(T& value)
	{
		lock_guard<mutex> lock(_mutex);
		if (_queue.empty())
		{
			return false;
		}

		value = std::move(_queue.front());
		_queue.pop();

		return true;
	}

	void WaitPop(T& value)
	{
		unique_lock<mutex> lock(_mutex);
		_conVar.wait(lock, [this] { return _queue.empty() == false;  });

		value = std::move(_queue.front());
		_queue.pop();
	}

private:
	queue<T> _queue;
	mutex _mutex;
	condition_variable _conVar;
};

//template<typename T>
//class LockFreeQueue
//{
//	struct Node
//	{
//		std::shared_ptr<T> data;
//		Node* next = nullptr;
//	};
//public:
//	LockFreeQueue()
//		: _head(new Node)
//		, _tail(_head)
//	{
//	}
//
//	LockFreeQueue(const LockFreeQueue&) = delete;
//	LockFreeQueue& operator = (const LockFreeQueue&) = delete;
//
//	void Push(const T& value)
//	{
//		std::shared_ptr<T> newData = make_shared<T>(value);
//
//		Node* dummy = new Node();
//
//		Node* oldTail = _tail;
//		oldTail->data.swap(newData);
//		oldTail->next = dummy;
//
//		_tail = dummy;
//	}
//
//	std::shared_ptr<T> TryPop()
//	{
//		Node* oldHead = PopHead();
//
//		if (oldHead == nullptr)
//		{
//			return std::shared_ptr<T>();
//		}
//
//		std::shared_ptr<T> res(oldHead->data);
//		delete oldHead;
//		return res;
//	}
//
//private:
//	Node* PopHead()
//	{
//		Node* oldHead = _head;
//
//		if (oldHead == _tail)
//		{
//			return nullptr;
//		}
//
//		_head = oldHead->next;
//		return oldHead;
//	}
//
//private:
//	// [data][data][data][data][data][data][data][data]
//	// [head][tail]
//	// [] ���̳�带 �������Ѵ�.head tail�� ���� ��带 ����Ű�� ������ �������
//	// [data][]
//	// [head][tail]
//	Node* _head;
//	Node* _tail;
//};

template<typename T>
class LockFreeQueue
{
	struct Node;

	struct CountedNodePtr
	{
		int32 externalCount; // ������
		Node* ptr = nullptr;
	};

	struct NodeCounter
	{
		uint32 internalCount : 30; // ������ ��ȯ ����
		uint32 externalCountRemaining : 2; // Push & Pop ���� ������ ����
	};

	struct Node
	{
		Node()
		{
			NodeCounter newCount;
			newCount.internalCount;
			newCount.externalCountRemaining = 2;
			count.store(newCount);

			next.ptr = nullptr;
			next.externalCount = 0;
		}
		
		atomic<T*> data;
		atomic<NodeCounter> count;
		CountedNodePtr next;
	};

public:
	LockFreeQueue()
		: _head(new Node)
		, _tail(_head)
	{
	}

	LockFreeQueue(const LockFreeQueue&) = delete;
	LockFreeQueue& operator = (const LockFreeQueue&) = delete;

	void Push(const T& value)
	{
		//std::shared_ptr<T> newData = make_shared<T>(value);

		//Node* dummy = new Node();

		//Node* oldTail = _tail;
		//oldTail->data.swap(newData);
		//oldTail->next = dummy;

		//_tail = dummy;

		unique_ptr<T> newData = make_unique<T>(value);
		
		CountedNodePtr dummy;
		dummy.ptr = new Node;
		dummy.externalCount = 1;

		CountedNodePtr oldTail = _tail.load(); // ptr nullptr

		while (true)
		{
			// ������ ȹ�� (externalCount�� ������ ���� +1 �� �갡 �̱�)
			IncreaseExternalCount(_tail, oldTail);

			// ������ ȹ�� (data�� ���� ��ȯ�� �ְ� �̱�)
		}


	}

	std::shared_ptr<T> TryPop()
	{
		//Node* oldHead = PopHead();

		//if (oldHead == nullptr)
		//{
		//	return std::shared_ptr<T>();
		//}

		//std::shared_ptr<T> res(oldHead->data);
		//delete oldHead;
		//return res;
	}

private:
	//Node* PopHead()
	//{
	//	Node* oldHead = _head;

	//	if (oldHead == _tail)
	//	{
	//		return nullptr;
	//	}

	//	_head = oldHead->next;
	//	return oldHead;
	//}

	static void IncreaseExternalCount(atomic<CountedNodePtr>& counter, CountedNodePtr oldCounter)
	{
		while (true)
		{
			CountedNodePtr newCounter = oldCounter;
			++newCounter.externalCount;

			if (counter.compare_exchagne_strong(oldCounter, newCounter))
			{
				oldCounter.externalCount = newCounter.externalCount;
				break;
			}
		}
	}

private:
	atomic<CountedNodePtr> _head;
	atomic<CountedNodePtr> _tail;
};
