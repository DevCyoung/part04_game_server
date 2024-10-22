#pragma once
#include <mutex>

class Account
{

};

class AccountManager
{
public:
	static AccountManager* Instance()
	{
		static AccountManager instance;
		return &instance;
	}

	Account* GetAccount(int32 id)
	{
		std::lock_guard<std::mutex> guard(_mutex);
		//������ �����
		return nullptr;
	}

	void ProcessLogin();

private:
	std::mutex _mutex;

};

