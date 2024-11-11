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

	// Overlapped IO (비동기 + 논블로킹)
	// Overlapped 함수를 건다 (WSARecv, WSASend)
	// Overlapped 함수가 성공했는지 확인 후
	// -> 성공했으면 결과 얻어서 처리
	// -> 실패했으면 사유를 확인 (pending 상태일 수 있음)
	//		-> 아직 준비가 되지않은상태.
	//		-> 완료시 알림받기

	// WSASend
	// WSARecv

	// 1) 비동기 입출력 소켓
	// 2) 버퍼 (배열의 시작주소), 개수
	// 3) 보내고/받은 바이트 수
	// 4) 상세 옵션인데 0
	// 5) WSAOVERLAPPED 구조체 주소값
	// 6) 입출력이 완료되면 OS 호출할 콜백 함수, 일단 오늘은 무시 5번사용할것
	//char sendBuffer[100];
	//WSABUF wsaBuf[2];
	//wsaBuf[0].buf = sendBuffer;
	//wsaBuf[0].len = 100;

	//char sendBuffer2[100];
	//wsaBuf[1].buf = sendBuffer2;
	//wsaBuf[1].len = 100;

	// Overlapped 모델 (이벤트 기반)
	// 비동기 입추력 지원하는 소켓 생성
	// 비동기 입출력 함수 호출 (완료 루틴의 시작 주소를 넘겨준다)
	// 비동기 작업이 바로 완료되지 않으면, WSA_IO_PENDING 오류 코드는 실패, 그외는 당장완료
	// 운영체제는 이벤트 객체를 signaled 상태로 만들어서 완료 상태 알려줌
	// WSAWaitForMultipleEvents 함수 호출해서 이벤트 객체의 signal 판별

	// 1) 비동기 소켓
	// 2) 넘겨준 overlapped 구조체
	// 3) 전송된 바이트 수
	// 4) 비동기 입출력 작업이 끝날때까지 대기할지? 안할지?
	// false
	// 5) 비동기 입출력 작업 관련 부가정보, 거의 사용 안 함.
	// WSAGetOverlappedResult 호출해서 비동기 입출력 결과 확인 및 데이터 처리

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

			// 문제있는 상황
			return 0;
		}

		Session session{ clientSocket };
		WSAEVENT wsaEvent = ::WSACreateEvent();
		session.overapped.hEvent = wsaEvent;

		std::cout << "Client Connected!" << std::endl;

		while (true)
		{
			WSABUF wsaBuf; // 얘는 없어져도됨
			wsaBuf.buf = session.recvBuffer; //이건절대건들면안됨
			wsaBuf.len = BUFSIZE;

			DWORD recvLen = 0;
			DWORD flags = 0;

			//비동기 방식의 Recv
			if (SOCKET_ERROR == ::WSARecv(clientSocket, &wsaBuf, 1, &recvLen, &flags, &session.overapped, nullptr))
			{
				// pending 상태, 혹시 나중에라도 데이터가 오면 나한테 알려주세요 예약을 검
				if (::WSAGetLastError() == WSA_IO_PENDING) // 문제는 아니고 지연중입니다. 받을게 없어요~
				{
																			//모두 기다림		//무한대로 대기
					::WSAWaitForMultipleEvents(1, &wsaEvent, TRUE, WSA_INFINITE, FALSE);

					//여기에서 값을 다시채움
					::WSAGetOverlappedResult(session.socket, &session.overapped, &recvLen, FALSE, &flags);
				}
			}

			//한번에 성공
			std::cout << "Data Recv Len = " << recvLen << std::endl;
		}		
	}


	::WSACleanup();
}
