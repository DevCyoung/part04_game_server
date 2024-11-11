#include <vector>
#include <thread>
#include <chrono>
#include <iostream>
#include <WinSock2.h>
#include <MSWSock.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#include <ServerHelper.h>
const int BUFSIZE = 1000;
#include <thread>
#include <chrono>
#include <queue>


#include <ThreadManager.h>
ThreadManager GTrheadManager;

struct Session
{	
	SOCKET socket = INVALID_SOCKET;
	char recvBuffer[BUFSIZE] = {};
	int recvBytes = 0;
};

enum IO_TYPE
{
	READ,
	WRITE,
	ACCEPT,
	CONNECT
};

struct OverlappedEx
{
	WSAOVERLAPPED overapped = {};
	int type = 0; // read , write, accept, connect
};

void WorkerThreadMain(HANDLE iocpHandle)
{
	while (true)
	{
		//완료된 일감 체크
		DWORD bytesTransferred = 0;
		Session* session = nullptr;
		OverlappedEx* overlappedEx = nullptr;

		BOOL ret = ::GetQueuedCompletionStatus(iocpHandle, &bytesTransferred, 
			(ULONG_PTR*)&session, 
			(LPOVERLAPPED*)&overlappedEx
			, INFINITE);

		if (ret == FALSE || bytesTransferred == 0)
		{
			// TODO : 연결 끊김
			continue;
		}

		 //CRASH overlappedEx->type
		std::cout << "Recv Data IOCP = " << bytesTransferred << std::endl;


		WSABUF wsaBuf;
		wsaBuf.buf = session->recvBuffer;
		wsaBuf.len = BUFSIZE;

		//태초 등록 한번 등록해야함
		DWORD recvLen = 0;
		DWORD flags = 0;
		//call back 사용X 낚시대에 비유, 물에 던지고 기다렸다가 끌어올리고 다시 낚시대를 물속에던짐
		::WSARecv(session->socket, &wsaBuf, 1, &recvLen, &flags, &overlappedEx->overapped, NULL); //Recv를 걸어줌
	}
}

int main()
{
	WSAData wsaData;
	if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		return 0;

	SOCKET listenSocket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (listenSocket == INVALID_SOCKET)
		return 0;

	//논블로킹으로 만들 필요도없다.
	//u_long on = 1;
	//if (::ioctlsocket(listenSocket, FIONBIO, &on) == INVALID_SOCKET)
	//	return 0;

	SOCKADDR_IN serverAddr;
	::memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = ::htonl(INADDR_ANY);
	serverAddr.sin_port = ::htons(7777);

	if (::bind(listenSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
		return 0;

	if (::listen(listenSocket, SOMAXCONN) == SOCKET_ERROR)
		return 0;

	std::cout << "Accept" << std::endl;

	// Overlapped 모델 
	// 비동기 입출력 함수 완료되면, 쓰레드 마다 있는 APC 큐에 일감이 쌓임
	// Alertable Wait 상태로 들어가서 APC 큐를 비우기 (콜백 호출)
	// 단점) APC큐 쓰레드마다 있다. Alertable Wait 자체도 조금 부담!
	// 단점) 이벤트 방식 소켓: 이벤트 1:1대응

	// IOCP (Completion Port) 모델
	// APC -> Completion Port (쓰레드 마다 있는건 아니고 유일하게 하나 다수를 만들수 있지만 하나만 만듬)
	// Alertable Wait -> CP 결과 처리를 GetQueuedCompletionStatus
	// 스레드랑 궁합이 굉장히 좋다

	// CreateIoCompletionPort
	// GetQueuedCompletionStatus

	// CP 생성
	HANDLE iocpHandle = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

	// worker Threads
	// 완료된 입출력들을 처리
	for (int i = 0; i < 100; ++i)
	{
		GTrheadManager.Launch([=]() {
			WorkerThreadMain(iocpHandle);
			});
	}

	std::vector<Session*> sessionManager;
	while (true)
	{
		SOCKADDR_IN clientAddr;
		int addrLen = sizeof(clientAddr);

		SOCKET clientSocket;
		clientSocket = ::accept(listenSocket, (sockaddr*)&clientAddr, &addrLen);
		if (INVALID_SOCKET == clientSocket)
		{
			return 0;
		}
		

		Session* session = new Session();
		session->socket = clientSocket;
		sessionManager.push_back(session);

		std::cout << "Client Connected!" << std::endl;

		// 소켓을 CP에 등록
		// 함수를 그대로사용 등록하는 용도																													0 으로하면 알잘딱
		::CreateIoCompletionPort((HANDLE)clientSocket, iocpHandle, /**/(ULONG_PTR)session, 0);

		WSABUF wsaBuf;
		wsaBuf.buf = session->recvBuffer;
		wsaBuf.len = BUFSIZE;

		OverlappedEx* overlappedEx = new OverlappedEx();
		overlappedEx->type = IO_TYPE::READ;


		//태초 등록 한번 등록해야함
		DWORD recvLen = 0;
		DWORD flags = 0;
																																							//call back 사용X
		::WSARecv(clientSocket, &wsaBuf, 1, &recvLen, &flags, &overlappedEx->overapped, NULL); //Recv를 걸어줌
	}

	GTrheadManager.Join();

	::WSACleanup();
}
