//#include "pch.h"
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
	
	//TCP
	//굳이적지않아도 TCP UDP를, 0으로 입력하면 알아서 골라줌
	//소켓은 포인터일뿐임
	SOCKET socket = ::socket(AF_INET, SOCK_STREAM, NULL);
	if (socket == INVALID_SOCKET)
	{
		int errCode = ::WSAGetLastError();
		std::cout << errCode << " : INVALID_SOCKET" << std::endl;
		return 0;
	}

	SOCKADDR_IN service;
	::memset(&service, 0, sizeof(SOCKADDR_IN));
	service.sin_family = AF_INET;
	//service.sin_addr.s_addr = inet_addr("127.0.0.1"); // 그냥 4바이트정수 inet_addr은 구식함수
	//naver.com
	::inet_pton(AF_INET, "127.0.0.1", &service.sin_addr);
	service.sin_port = htons(7878); //host to network short 엔디안 이슈
	//Littl-Endian vs Big-Endian
	// 0x12345678
	// low [78][56][34][12] 리틀엔디안
	// low [12][34][56][78] 빅엔디안 = 네트워크 표준 중간중간 장비들도 기계다보니까 두가지 방식중 하나를 골라서 사용해야함
	// 때문에 한가지정책을 골라서 일관성있게 사용해야한다.
	// 대부분은 리틀엔디안


	//버전이 여러개기 때문에 이렇게하는것
	if (SOCKET_ERROR == ::connect(socket, (struct sockaddr*)&service, sizeof(SOCKADDR_IN)))
	{
		int errCode = ::WSAGetLastError();
		std::cout << errCode << " : Connect Fail" << std::endl;
		return 0;
	}

	std::cout << "Connected To Server !" << std::endl;

	while (true)
	{
		//std::this_thread::sleep_for(1ms);
	}



	/*if (SOCKET_ERROR == ::bind(socket, (struct sockaddr*)&service, sizeof(SOCKADDR_IN)))
	{
		int32 errCode = ::WSAGetLastError();
		std::cout << errCode << " : SOCKET_ERROR" << std::endl;
		return 0;
	}	*/
	

	//::bind(socket, )

	::WSACleanup();
}
