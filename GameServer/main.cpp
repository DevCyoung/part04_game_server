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
#include "TypeCast.h"

using TL = TypeList<class Player, class Charactor, class Knight, class  QD>;

class Player
{
public:
	Player()
	{
		INIT_TL(Player);
	}

	DECLARE_TL;
};

class Charactor : public Player
{
public:
	Charactor() { INIT_TL(Charactor); };
};

class Knight : public Player
{
public:
	Knight() { INIT_TL(Knight); }
};

class QD
{

};

int main()
{
	Player* player = new Player();

	Knight* knight = TypeCast<Knight*>(player);

	if (knight)
	{

	}

	Player* p = TypeCast<Player*>(knight);


	Player* knight2 = new Knight();


	bool can = CanCast<Knight*>(knight2);


	if (knight)
	{

	}
}