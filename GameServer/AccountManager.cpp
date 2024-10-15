#include "pch.h"
#include "AccountManager.h"
#include "UserManager.h"

void AccountManager::ProcessLogin()
{
	// accountLock
	User* user = UserManager::Instance()->GetUser(100);

	lock_guard<mutex> guard(_mutex);
	lock_guard<mutex> g1(_mutex, std::adopt_lock);
}
