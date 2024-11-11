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

struct Session
{
	SOCKET socket = INVALID_SOCKET;
	char recvBuffer[BUFSIZE] = {};
	int recvBytes = 0;
	WSAOVERLAPPED overapped;
};

int main()
{
	WSAData wsaData;
	if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		return 0;

	SOCKET listenSocket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (listenSocket == INVALID_SOCKET)
		return 0;

	u_long on = 1;
	if (::ioctlsocket(listenSocket, FIONBIO, &on) == INVALID_SOCKET)
		return 0;

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

	// Overlapped IO (�񵿱� + ����ŷ)
	// Overlapped �Լ��� �Ǵ� (WSARecv, WSASend)
	// Overlapped �Լ��� �����ߴ��� Ȯ�� ��
	// -> ���������� ��� �� ó��
	// -> ���������� ������ Ȯ�� (pending ������ �� ����)
	//		-> ���� �غ� ������������.
	//		-> �Ϸ�� �˸��ޱ�

	// WSASend
	// WSARecv

	// 1) �񵿱� ����� ����
	// 2) ���� (�迭�� �����ּ�), ����
	// 3) ������/���� ����Ʈ ��
	// 4) �� �ɼ��ε� 0
	// 5) WSAOVERLAPPED ����ü �ּҰ�
	// 6) ������� �Ϸ�Ǹ� OS ȣ���� �ݹ� �Լ�, �ϴ� ������ ���� 5������Ұ�
	//char sendBuffer[100];
	//WSABUF wsaBuf[2];
	//wsaBuf[0].buf = sendBuffer;
	//wsaBuf[0].len = 100;

	//char sendBuffer2[100];
	//wsaBuf[1].buf = sendBuffer2;
	//wsaBuf[1].len = 100;

	// Overlapped �� (�̺�Ʈ ���)
	// �񵿱� ���߷� �����ϴ� ���� ����
	// �񵿱� ����� �Լ� ȣ�� (�Ϸ� ��ƾ�� ���� �ּҸ� �Ѱ��ش�)
	// �񵿱� �۾��� �ٷ� �Ϸ���� ������, WSA_IO_PENDING ���� �ڵ�� ����, �׿ܴ� ����Ϸ�
	// �ü���� �̺�Ʈ ��ü�� signaled ���·� ���� �Ϸ� ���� �˷���
	// WSAWaitForMultipleEvents �Լ� ȣ���ؼ� �̺�Ʈ ��ü�� signal �Ǻ�

	// 1) �񵿱� ����
	// 2) �Ѱ��� overlapped ����ü
	// 3) ���۵� ����Ʈ ��
	// 4) �񵿱� ����� �۾��� ���������� �������? ������?
	// false
	// 5) �񵿱� ����� �۾� ���� �ΰ�����, ���� ��� �� ��.
	// WSAGetOverlappedResult ȣ���ؼ� �񵿱� ����� ��� Ȯ�� �� ������ ó��

	while (true)
	{
		SOCKADDR_IN clientAddr;
		int addrLen = sizeof(clientAddr);

		SOCKET clientSocket;
		while (true)
		{
			clientSocket = ::accept(listenSocket, (sockaddr*)&clientAddr, &addrLen);
			if (clientSocket != INVALID_SOCKET)
			{
				break;
			}

			if (WSAEWOULDBLOCK == ::WSAGetLastError())
			{
				continue;
			}

			// �����ִ� ��Ȳ
			return 0;
		}

		Session session{ clientSocket };
		WSAEVENT wsaEvent = ::WSACreateEvent();
		session.overapped.hEvent = wsaEvent;

		std::cout << "Client Connected!" << std::endl;

		while (true)
		{
			WSABUF wsaBuf; // ��� ����������
			wsaBuf.buf = session.recvBuffer; //�̰�����ǵ��ȵ�
			wsaBuf.len = BUFSIZE;

			DWORD recvLen = 0;
			DWORD flags = 0;

			//�񵿱� ����� Recv
			if (SOCKET_ERROR == ::WSARecv(clientSocket, &wsaBuf, 1, &recvLen, &flags, &session.overapped, nullptr))
			{
				// pending ����, Ȥ�� ���߿��� �����Ͱ� ���� ������ �˷��ּ��� ������ ��
				if (::WSAGetLastError() == WSA_IO_PENDING) // ������ �ƴϰ� �������Դϴ�. ������ �����~
				{
																			//��� ��ٸ�		//���Ѵ�� ���
					::WSAWaitForMultipleEvents(1, &wsaEvent, TRUE, WSA_INFINITE, FALSE);

					//���⿡�� ���� �ٽ�ä��
					::WSAGetOverlappedResult(session.socket, &session.overapped, &recvLen, FALSE, &flags);
				}
			}

			//�ѹ��� ����
			std::cout << "Data Recv Len = " << recvLen << std::endl;
		}		
	}


	::WSACleanup();
}
