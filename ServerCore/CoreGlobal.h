#pragma once

extern class ThreadManager* GThreadManager;
extern class DeadLockProfiler* GDeadLockProfiler;

class CoreGlobal
{
public:
	CoreGlobal();
	virtual ~CoreGlobal();
};
