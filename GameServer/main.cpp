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

CoreGlobal GCore;

int main()
{
	std::unique_ptr<int32> p = make_unique<int32>(new int32);

	std::shared_ptr<int32>
}