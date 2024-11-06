#include <vector>
#include <thread>
#include <chrono>
#include <iostream>
#include <WinSock2.h>
#include <MSWSock.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

int main()
{
	//Winsock lib �ʱ�ȭ
	WORD wVersionRequested = MAKEWORD(2, 2);
	WSADATA wsaData;
	int err = ::WSAStartup(wVersionRequested, &wsaData);

	SOCKET socket = ::socket(AF_INET, SOCK_DGRAM, NULL);
	if (socket == INVALID_SOCKET)
	{
		int errCode = ::WSAGetLastError();
		std::cout << errCode << " : INVALID_SOCKET" << std::endl;
		return 0;
	}

	// �ɼ��� �ؼ��ϰ� ó���� ��ü? LEVEL
	// ���� �ڵ� -> SOL_SOCKEt
	// IPv44 -> IPPROTO_IP
	// TCP �������� -> IPPROTO_TCP

	//::setsockopt(socket, SOL_SOCKET, SO_RCVBUF, )

	// SO_KEEPALIVE = �ֱ������� ���� ���� Ȯ�� ���� (TCP Only)
	// ������ �Ҹ��ҹ����� ���� ���´ٸ�??
	// �ֱ������� TCP �������� ���� ���� Ȯ�� -> ������ ���� ����
	bool enable = true;
	::setsockopt(socket, SOL_SOCKET, SO_KEEPALIVE, (const char*)&enable, sizeof(enable));

	// SO_LINGER �����ϴ�	
	// ���� ���ҽ� ��ȯ
	// send -> closesocket
	// send ���Ǹ鼭 ������ �Ǿ������ٵ�...������� �ٺ����� ������ �������ΰ� ? ������ ��å
	//::closesocket(socket);

	// onoff = 0 �̸� closesocket() �� �ٷ� ����, �ƴϸ� linger�ʸ�ŭ ��� 
	LINGER linger;
	linger.l_onoff = 1;
	linger.l_linger = 5; //��� �ð�, ������� �����ΰ� ������ �������� �������� ��ٷȴٰ� closesocket��
	::setsockopt(socket, SOL_SOCKET, SO_LINGER, (const char*)&linger, sizeof(linger));

	//�׳� Socket�� �ݴ°��� �ųʰ����°�
	//Half_Close
	// SD_SEND : send ���´� (sendbuffer���� �����°��� ����)
	// SD_RECEIVE recv ���´�
	// SD_BOTH �Ѵ� ���´�
	//::shutdown(socket, SD_SEND); Recv Size�� 0���ο´�. closesocket������ �ؾ���

	// SO_SNDBUF = �۽� ���� ũ��
	// SO_RCVBUF = ���� ���� ũ��

	int sendBuffSize = 123456789;
	int optionLen = sizeof(sendBuffSize);
	::setsockopt(socket, SOL_SOCKET, SO_SNDBUF, (const char*)&sendBuffSize, optionLen);	
	sendBuffSize = 0;

	
	
	
	::getsockopt(socket, SOL_SOCKET, SO_SNDBUF, (char*)&sendBuffSize, &optionLen);
	std::cout << "�۽Ź��� ũ�� " << sendBuffSize << std::endl;

	int recvBuffSize;
	optionLen = sizeof(recvBuffSize);
	::getsockopt(socket, SOL_SOCKET, SO_RCVBUF, (char*)&recvBuffSize, &optionLen);
	std::cout << "���Ź��� ũ�� " << recvBuffSize << std::endl;

	// SO_REUSEADDR
	// IP�ּ� �� port ����
	// ���ߴܰ迡�� ���� �̹� �־ ������ ���ϻ��
	{
		bool enable = true;
		::setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, (const char*)&enable, sizeof(enable));
	}

	// IPPROTO_TCP
	// TCP_NODELAY = Nagle ���̱� �˰��� �۵� ����
	// �����Ͱ� ����� ũ�� ������, �׷��� ������ �����Ͱ� ����� ���϶����� ���
	// ���� : ���� ��Ŷ�� ���ʿ��ϰ� ���� �����Ǵ� ���� ����
	// ���� : ���� �ð� ����
	// �����ð��� ���߿��ϱ⶧���� Nagle�� ���°� �Ϲ����̴�.
	//  NoDelay �� True�� �ϸ� nagle�� �۵������ʴ´�.
	{
		bool enable = true;
		::setsockopt(socket, IPPROTO_TCP, TCP_NODELAY, (const char*)&enable, sizeof(enable));
	}




	while (true);

	::WSACleanup();
}