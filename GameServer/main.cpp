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
	//조금더 우아한 방법
	// 

	// [T* | ReCountBlocking*] 같은공간에 낑겨넣는다.
	std::shared_ptr<Player> spr3(new Player);
	std::shared_ptr<Player> spr = make_shared<Player>();
	std::weak_ptr<Player> wpr = spr;

	//존재하는지 체크해야함
	bool expired =  wpr.expired();

	std::shared_ptr<Player> spr2 = wpr.lock(); //없으면(sharedCount가 0이면) null 상태가 됨
	//weakCount가 0이 아니면 RefCountingBlock은 아직 안날라간다.

	//[T*][ReCountBlocking*] 동일 한 레퍼런스 동일판 포인터 블럭
	std::shared_ptr<Player> spr2(new Player);

	//Shared 와 Weak는 서로 상충
	//Weak는 Night 수명과는 연관이없다.
}