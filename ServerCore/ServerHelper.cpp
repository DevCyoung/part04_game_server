#include "pch.h"
#include "ServerHelper.h"
#define BUFFER_SIZE_SERVER 5000000
ServerHelper::ServerHelper()
{
	mRecvBuffer = new char[BUFFER_SIZE_SERVER];
	mSendBuffer = new char[BUFFER_SIZE_SERVER];
}

ServerHelper::~ServerHelper()
{
	delete[] mRecvBuffer;
	delete[] mSendBuffer;
}

void ServerHelper::selectUpdate()
{
	fd_set reads;
	fd_set writes;

	while (true)
	{
		{
			std::lock_guard<std::mutex> lockGuard(mlock);

			FD_ZERO(&reads);
			FD_ZERO(&writes);

			for (YSession& s : mLisetnSockets)
			{
				FD_SET(s.Socket, &reads);
			}

			for (YSession& s : mVecOri)
			{
				if (!s.bConnect)
				{
					continue;
				}

				if (s.IOType & eYSIO::RECV)
				{
					FD_SET(s.Socket, &reads);
				}

				if ((s.IOType & eYSIO::SEND))
				{
					FD_SET(s.Socket, &writes);
				}
			}
		}

		int retVal = ::select(0, &reads, &writes, nullptr, nullptr);
		std::lock_guard<std::mutex> lockGuard(mlock);

		for (YSession& s : mLisetnSockets)
		{
			if (s.bConnect)
			{
				continue;
			}

			if (FD_ISSET(s.Socket, &reads))
			{
				s.bConnect = true;
				SOCKADDR_IN clientAddr;
				int addrLen = sizeof(clientAddr);
				SOCKET clientsocket = ::accept(s.Socket, (SOCKADDR*)&clientAddr, &addrLen);				

				if (clientsocket != INVALID_SOCKET)
				{
					YSession session;
					session.Socket = clientsocket;
					session.bConnect = true;
					session.Symbol = s.Symbol;
					session.IOType = s.IOType;
					mVecOri.push_back(session);
				}
			}
		}

		for (YSession& s : mVecOri)
		{
			if (!s.bConnect)
			{
				continue;
			}

			if (FD_ISSET(s.Socket, &reads))
			{
				int recvLen = ::recv(s.Socket, mRecvBuffer, BUFFER_SIZE_SERVER, 0);

				if (recvLen <= 0)
				{			
					continue;
				}

				s.RecvBuff.insert(s.RecvBuff.end(), mRecvBuffer, mRecvBuffer + recvLen);
			}

			if (FD_ISSET(s.Socket, &writes))
			{
				int sendLen = ::send(s.Socket, s.SendBuff.data(), s.SendBuff.size(), 0);

				if (sendLen <= 0)
				{
					continue;
				}

				s.SendBuff.erase(s.SendBuff.begin(), s.SendBuff.begin() + sendLen);
			}
		}
	}
}

void ServerHelper::StartThread()
{
	mThread = std::thread(&ServerHelper::selectUpdate, this);
}

void ServerHelper::Update()
{
	std::lock_guard<std::mutex> lockGuard(mlock);

	for (YSession& s : mVecOri)
	{
		std::vector<char>& vecRecv = mMapRecvBuffer[s.Symbol];
		vecRecv.insert(vecRecv.end(), s.RecvBuff.data(), s.RecvBuff.data() + s.RecvBuff.size());

		if (s.bConnect)
		{
			std::vector<char>& vecSend = mMapSendBuffer[s.Symbol];
			s.SendBuff.insert(s.SendBuff.end(), vecSend.data(), vecSend.data() + vecSend.size());
		}

		s.RecvBuff.clear();		
		mMapSendBuffer[s.Symbol].clear();
	}

	for (YSession& s : mVecOri)
	{
		if (s.bConnect)
		{
			continue;
		}

		SOCKADDR_IN sockAddr;
		sockAddr.sin_family = AF_INET;
		sockAddr.sin_port = htons(s.OtherPort);
		::inet_pton(AF_INET, s.OtherIP.c_str(), &sockAddr.sin_addr);

		int ret = ::connect(s.Socket, (sockaddr*)&sockAddr, sizeof(sockAddr));
		int err = WSAGetLastError();

		if (ret < 0 && err != EINPROGRESS)
		{
			s.bConnect = true;
		}
	}
}

void ServerHelper::AddLitener(YSession session)
{
	mLisetnSockets.push_back(session);
}

void ServerHelper::AddClient(YSession session)
{
	mVecOri.push_back(session);
}

int ServerHelper::Read(const char* symbol, void* message, const int messageSize)
{
	std::vector<char>& vecRecv = mMapRecvBuffer[symbol];

	int ret = -1;
	int ms = vecRecv.size() >= messageSize ? messageSize : vecRecv.size();
	memcpy_s(message, ms, vecRecv.data(), ms);
	vecRecv.erase(vecRecv.begin(), vecRecv.begin() + ms);
	ret = ms;
	return ret;
}

int ServerHelper::Write(const char* symbol, const void* message, const int messageSize)
{
	int ret = -1;
	std::vector<char>& vecSend = mMapSendBuffer[symbol];
	vecSend.insert(vecSend.end(), (const char*)message, (const char*)message + messageSize);
	return vecSend.size();
}
