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
public:
	Charactor()
	{
		std::cout << "Charactor" << std::endl;
	}
};

class Player : public Charactor
{
public:
	int hp;
};

std::vector<Charactor> getCharactors()
{
	std::vector<Charactor> ch;

	ch = std::vector<Charactor>(10);

	return ch;
}

int main()
{
	for (int32 i = 0; i < 5; i++)
	{
		GThreadManager->Launch([]()
			{
				while (true)
				{
					Vector<Charactor> v(10);

					Map<int32, Charactor> m;
					m[100] = Charactor();

					this_thread::sleep_for(10ms);
				}
			});
	}

	//__int64 a;


	GThreadManager->Join();
}