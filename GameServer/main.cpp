#include "pch.h"
#include <thread>
#include <atomic>

#include <mutex>

vector<int32> v;

// Mutual Exclusive ��ȣ��Ÿ��
mutex m;

// RAII (Resource Acquisition is initialzation)
template<typename T>
class LockGuard
{
public:
	LockGuard(T& m)
	{
		_mutex = &m;
		_mutex->lock();
	}

	~LockGuard()
	{
		_mutex->unlock();
	}

private:
	T* _mutex;

};

void Push()
{
	for (int32 i = 0; i < 10000; ++i)
	{
		//�ڹ�����ױ�
		//LockGuard<std::mutex> lockGuard(m);
		//std::lock_guard<std::mutex> lockGuard(m);
		std::unique_lock<std::mutex> uniqueLock(m, std::defer_lock);
		uniqueLock.lock();

		//�ڹ��� ��ױ�
		//m.lock();		
		//
		//v.push_back(i);
		//
		////lock��Ǯ�� ��������
		////������ϸ� �����
		//
		////�ڹ��� Ǯ��
		//m.unlock();
	}
}

int main()
{	
	//Crash!
	std::thread t1(Push);
	std::thread t2(Push);

	t1.join();
	t2.join();

	cout << v.size() << endl;
}