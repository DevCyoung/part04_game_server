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
		//�Ϸ�� �ϰ� üũ
		DWORD bytesTransferred = 0;
		Session* session = nullptr;
		OverlappedEx* overlappedEx = nullptr;

		BOOL ret = ::GetQueuedCompletionStatus(iocpHandle, &bytesTransferred, 
			(ULONG_PTR*)&session, 
			(LPOVERLAPPED*)&overlappedEx
			, INFINITE);

		if (ret == FALSE || bytesTransferred == 0)
		{
			// TODO : ���� ����
			continue;
		}

		 //CRASH overlappedEx->type
		std::cout << "Recv Data IOCP = " << bytesTransferred << std::endl;


		WSABUF wsaBuf;
		wsaBuf.buf = session->recvBuffer;
		wsaBuf.len = BUFSIZE;

		//���� ��� �ѹ� ����ؾ���
		DWORD recvLen = 0;
		DWORD flags = 0;
		//call back ���X ���ô뿡 ����, ���� ������ ��ٷȴٰ� ����ø��� �ٽ� ���ô븦 ���ӿ�����
		::WSARecv(session->socket, &wsaBuf, 1, &recvLen, &flags, &overlappedEx->overapped, NULL); //Recv�� �ɾ���
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

	//����ŷ���� ���� �ʿ䵵����.
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

	// Overlapped �� 
	// �񵿱� ����� �Լ� �Ϸ�Ǹ�, ������ ���� �ִ� APC ť�� �ϰ��� ����
	// Alertable Wait ���·� ���� APC ť�� ���� (�ݹ� ȣ��)
	// ����) APCť �����帶�� �ִ�. Alertable Wait ��ü�� ���� �δ�!
	// ����) �̺�Ʈ ��� ����: �̺�Ʈ 1:1����

	// IOCP (Completion Port) ��
	// APC -> Completion Port (������ ���� �ִ°� �ƴϰ� �����ϰ� �ϳ� �ټ��� ����� ������ �ϳ��� ����)
	// Alertable Wait -> CP ��� ó���� GetQueuedCompletionStatus
	// ������� ������ ������ ����

	// CreateIoCompletionPort
	// GetQueuedCompletionStatus

	// CP ����
	HANDLE iocpHandle = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

	// worker Threads
	// �Ϸ�� ����µ��� ó��
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

		// ������ CP�� ���
		// �Լ��� �״�λ�� ����ϴ� �뵵																													0 �����ϸ� ���ߵ�
		::CreateIoCompletionPort((HANDLE)clientSocket, iocpHandle, /**/(ULONG_PTR)session, 0);

		WSABUF wsaBuf;
		wsaBuf.buf = session->recvBuffer;
		wsaBuf.len = BUFSIZE;

		OverlappedEx* overlappedEx = new OverlappedEx();
		overlappedEx->type = IO_TYPE::READ;


		//���� ��� �ѹ� ����ؾ���
		DWORD recvLen = 0;
		DWORD flags = 0;
																																							//call back ���X
		::WSARecv(clientSocket, &wsaBuf, 1, &recvLen, &flags, &overlappedEx->overapped, NULL); //Recv�� �ɾ���
	}

	GTrheadManager.Join();

	::WSACleanup();
}
