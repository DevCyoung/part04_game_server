#pragma once
#include "Types.h"
#include "MemoryPool.h"

template<typename T>
class ObjectPool
{
public:

private:
	static int32 sAllocSize;
	static MemoryPool sPool;
};
