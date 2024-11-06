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
	//Winsock lib 초기화
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

	// 옵션을 해석하고 처리할 주체? LEVEL
	// 소켓 코드 -> SOL_SOCKEt
	// IPv44 -> IPPROTO_IP
	// TCP 프로토콜 -> IPPROTO_TCP

	//::setsockopt(socket, SOL_SOCKET, SO_RCVBUF, )

	// SO_KEEPALIVE = 주기적으로 연결 상태 확인 여부 (TCP Only)
	// 상대방이 소리소문없이 연결 끊는다면??
	// 주기적으로 TCP 프로토콜 연결 상태 확인 -> 끊어진 연결 감지
	bool enable = true;
	::setsockopt(socket, SOL_SOCKET, SO_KEEPALIVE, (const char*)&enable, sizeof(enable));

	// SO_LINGER 지연하다	
	// 소켓 리소스 반환
	// send -> closesocket
	// send 가되면서 예약이 되어있을텐데...찌꺼끼들을 다보내고 연결을 끊을것인가 ? 에대한 정책
	//::closesocket(socket);

	// onoff = 0 이면 closesocket() 이 바로 리턴, 아니면 linger초만큼 대기 
	LINGER linger;
	linger.l_onoff = 1;
	linger.l_linger = 5; //대기 시간, 어느정도 텀을두고 나머지 정보들이 보내지길 기다렸다가 closesocket함
	::setsockopt(socket, SOL_SOCKET, SO_LINGER, (const char*)&linger, sizeof(linger));

	//그냥 Socket을 닫는것은 매너가없는것
	//Half_Close
	// SD_SEND : send 막는다 (sendbuffer에서 보내는것을 막음)
	// SD_RECEIVE recv 막는다
	// SD_BOTH 둘다 막는다
	//::shutdown(socket, SD_SEND); Recv Size가 0으로온다. closesocket이전에 해야함

	// SO_SNDBUF = 송신 버퍼 크기
	// SO_RCVBUF = 수신 버퍼 크기

	int sendBuffSize = 123456789;
	int optionLen = sizeof(sendBuffSize);
	::setsockopt(socket, SOL_SOCKET, SO_SNDBUF, (const char*)&sendBuffSize, optionLen);	
	sendBuffSize = 0;

	
	
	
	::getsockopt(socket, SOL_SOCKET, SO_SNDBUF, (char*)&sendBuffSize, &optionLen);
	std::cout << "송신버퍼 크기 " << sendBuffSize << std::endl;

	int recvBuffSize;
	optionLen = sizeof(recvBuffSize);
	::getsockopt(socket, SOL_SOCKET, SO_RCVBUF, (char*)&recvBuffSize, &optionLen);
	std::cout << "수신버퍼 크기 " << recvBuffSize << std::endl;

	// SO_REUSEADDR
	// IP주소 및 port 재사용
	// 개발단계에서 편함 이미 있어도 강제로 소켓사용
	{
		bool enable = true;
		::setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, (const char*)&enable, sizeof(enable));
	}

	// IPPROTO_TCP
	// TCP_NODELAY = Nagle 네이글 알고리즘 작동 여부
	// 데이터가 충분히 크면 보내고, 그렇지 않으면 데이터가 충분히 쌓일때까지 대기
	// 장점 : 작은 패킷이 불필요하게 많이 생성되는 일이 방지
	// 단점 : 반응 시간 손해
	// 반응시간이 더중요하기때문에 Nagle을 끄는게 일반적이다.
	//  NoDelay 를 True로 하면 nagle이 작동하지않는다.
	{
		bool enable = true;
		::setsockopt(socket, IPPROTO_TCP, TCP_NODELAY, (const char*)&enable, sizeof(enable));
	}




	while (true);

	::WSACleanup();
}