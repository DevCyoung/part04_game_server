#include <thread>
#include <chrono>
#include <iostream>
#include <WinSock2.h>
#include <MSWSock.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

int main()
{
	//Winsock lib 초기화
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
	//service.sin_addr.s_addr = ::htonl(INADDR_ANY); // 니가 알아서 골라줘 말이되는주소가 연결됨
	service.sin_port = htons(7878); //host to network short 엔디안 이슈

	if (SOCKET_ERROR == ::bind(listenSocket, (const sockaddr*)&service, sizeof(service)))
	{
		int errCode = ::WSAGetLastError();
		std::cout << errCode << " : Connect Fail" << std::endl;
		return 0;
	}

	//영업시작 백로그는 대기열
	::listen(listenSocket, 10);

	//손님을 받을수있는 상태암 여기서부터 커넥트가능	

	while (true)
	{
		SOCKADDR_IN clientAddr; // IPv4
		int addLen = sizeof(SOCKADDR_IN);
		memset(&clientAddr, 0, sizeof(clientAddr));
		//클라이언트의 정보를 얻을 수 있음
		//옵션임 넣어주지 않아도 상관없음

		SOCKET clientSocket = ::accept(listenSocket, (sockaddr*)&clientAddr, &addLen);

		if (INVALID_SOCKET == clientSocket)
		{
			int errCode = ::WSAGetLastError();
			std::cout << errCode << " : Accept Fail" << std::endl;
			return 0;
		}
		std::cout << "Accept Client !" << std::endl;

		//손님 입장
		char ipAddress[16];
		::inet_ntop(AF_INET, &clientAddr.sin_addr, ipAddress, sizeof(ipAddress));
		std::cout << "Client Connected! IP = " << ipAddress << std::endl;

		//Todo


		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

	::WSACleanup();
}