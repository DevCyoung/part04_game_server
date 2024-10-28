#pragma once
#include "Lock.h"

class PlayerManager
{
	USE_LOCK;

public:
	void PlayerThenAccount();
	void Lock();
};

extern PlayerManager GPlayerManager;
