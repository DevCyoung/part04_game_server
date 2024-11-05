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

	SOCKET listenSocket = ::socket(AF_INET, SOCK_STREAM, NULL);
	if (listenSocket == INVALID_SOCKET)
	{
		int errCode = ::WSAGetLastError();
		std::cout << errCode << " : INVALID_SOCKET" << std::endl;
		return 0;
	}

	SOCKADDR_IN service;
	::memset(&service, 0, sizeof(SOCKADDR_IN));
	service.sin_family = AF_INET;
	::inet_pton(AF_INET, "127.0.0.1", &service.sin_addr);
	//service.sin_addr.s_addr = ::htonl(INADDR_ANY); // �ϰ� �˾Ƽ� ����� ���̵Ǵ��ּҰ� �����
	service.sin_port = htons(7878); //host to network short ����� �̽�

	if (SOCKET_ERROR == ::bind(listenSocket, (const sockaddr*)&service, sizeof(service)))
	{
		int errCode = ::WSAGetLastError();
		std::cout << errCode << " : Connect Fail" << std::endl;
		return 0;
	}

	//�������� ��α״� ��⿭
	::listen(listenSocket, 10);

	//�մ��� �������ִ� ���¾� ���⼭���� Ŀ��Ʈ����	

	while (true)
	{
		SOCKADDR_IN clientAddr; // IPv4
		int addLen = sizeof(SOCKADDR_IN);
		memset(&clientAddr, 0, sizeof(clientAddr));
		//Ŭ���̾�Ʈ�� ������ ���� �� ����
		//�ɼ��� �־����� �ʾƵ� �������

		SOCKET clientSocket = ::accept(listenSocket, (sockaddr*)&clientAddr, &addLen);

		if (INVALID_SOCKET == clientSocket)
		{
			int errCode = ::WSAGetLastError();
			std::cout << errCode << " : Accept Fail" << std::endl;
			return 0;
		}
		std::cout << "Accept Client !" << std::endl;

		//�մ� ����
		char ipAddress[16];
		::inet_ntop(AF_INET, &clientAddr.sin_addr, ipAddress, sizeof(ipAddress));
		std::cout << "Client Connected! IP = " << ipAddress << std::endl;

		//Todo


		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

	::WSACleanup();
}