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

class Charactor
{

};

class Player : public Charactor
{
public:
	int hp;
};

int main()
{
	Player* player = xnew<Player>();
	player->hp = 100;

	xdelete(player);
	player->hp = 100;
}