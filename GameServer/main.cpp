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
#include "Memory.h"

//CoreGlobal GCore;

class Player
{
public:
	Player(int32 other)
	{

	}
	virtual void Pla() {};
};

class Player2 : public Player
{
	virtual void Pla() override {};
};

//_CrtDumpMemoryLeaks();

//void* operator new(size_t size)
//{
//	std::cout << "new! " << size << endl;
//	void* ptr = ::malloc(size);
//	return ptr;
//}
//
//void operator delete(void* ptr)
//{
//	std::cout << "delete!" << std::endl;
//	::free(ptr);
//}
//
//void* operator new[](size_t size)
//{
//	std::cout << "new[]! " << size << endl;
//	void* ptr = ::malloc(size);
//	return ptr;
//}
//
//void operator delete[](void* ptr)
//{
//	std::cout << "delete[]!" << std::endl;
//	::free(ptr);
//}


#define ynew(CLASS) (decltype(CLASS)*)new(BaseAllocator::Alloc(sizeof(decltype(CLASS))))CLASS
#define ynewa(CLASS) (decltype(CLASS)*)new(BaseAllocator::Alloc(sizeof(decltype(CLASS))))CLASS
#define ydeletea(pointer) BaseAllocator::Rlease(pointer)
//#define ynew(CLASS) new CLASS

int main()
{
	//xnew<Player>(123);

	//void* _memory = new(::malloc(sizeof(decltype(Player(32)))))Player(32);


	//Player* player = ynew(Player[24]);

	//ydelete(player);	

	new int[124];

}