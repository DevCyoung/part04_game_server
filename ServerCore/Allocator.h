#pragma once

/*------------------
* BaseAllocator
------------------*/
class BaseAllocator
{
public:
	static void*	Alloc(int32 size);
	static void		Rlease(void* ptr);
};

