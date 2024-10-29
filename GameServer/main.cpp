#include "pch.h"
#include <thread>
#include <atomic>
#include <mutex>
#include <Windows.h>
#include <assert.h>
#include <future>
#include "ConcurrentStack.h"
#include "ConcurrentQueue.h"
#include "ThreadManager.h"
#include "Lock.h"

//CoreGlobal GCore;

class Player
{

};

//_CrtDumpMemoryLeaks();

int main()
{
	using type = remove_extent_t<int32[123]>;

	//Shared, Weak
	//[T*][ReCountBlocking*]
	//���ݴ� ����� ���
	// 

	// [T* | ReCountBlocking*] ���������� ���ִܳ´�.
	std::shared_ptr<Player> spr3(new Player);
	std::shared_ptr<Player> spr = make_shared<Player>();
	std::weak_ptr<Player> wpr = spr;

	//�����ϴ��� üũ�ؾ���
	bool expired =  wpr.expired();

	std::shared_ptr<Player> spr2 = wpr.lock(); //������(sharedCount�� 0�̸�) null ���°� ��
	//weakCount�� 0�� �ƴϸ� RefCountingBlock�� ���� �ȳ��󰣴�.

	//[T*][ReCountBlocking*] ���� �� ���۷��� ������ ������ ��
	std::shared_ptr<Player> spr2(new Player);

	//Shared �� Weak�� ���� ����
	//Weak�� Night ������� �����̾���.
}