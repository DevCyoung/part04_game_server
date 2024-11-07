#pragma once

#include <thread>
#include <chrono>
#include <iostream>
#include <WinSock2.h>
#include <MSWSock.h>
#include <WS2tcpip.h>
#include <vector>
#include <map>
#include <thread>
#include <mutex>

enum eYSIO
{
	RECV = 0x1,
	SEND = 0x2,	
};

struct YSession
{
	SOCKET Socket;
	std::string Symbol;
	std::vector<char> RecvBuff;
	std::vector<char> SendBuff;
	unsigned int MyPort;
	std::string MyIP;
	unsigned int OtherPort;
	std::string OtherIP;
	int IOType;
	bool bConnect;
};

class ServerHelper
{
public:
	ServerHelper();
	~ServerHelper();
	
	void StartThread();
	void Update();

	void AddLitener(YSession session);
	void AddClient(YSession session);

	int Read(const char* symbol, void* message, const int messageSize);
	int Write(const char* symbol, const void* message, const int messageSize);

private:
	void selectUpdate();

	char* mRecvBuffer = nullptr;
	char* mSendBuffer = nullptr;

	//gaurd
	std::vector<YSession> mLisetnSockets;
	std::vector<YSession> mVecOri;
	std::thread mThread;
	//gaurd

	std::mutex	mlock;

	std::vector<YSession> mVecRecvCopy;
	std::vector<YSession> mVecSendCopy;
	

	std::map<std::string, std::vector<char>> mMapSendBuffer;
	std::map<std::string, std::vector<char>> mMapRecvBuffer;
};
