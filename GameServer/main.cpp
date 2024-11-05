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
	//Winsock lib �ʱ�ȭ
	WORD wVersionRequested = MAKEWORD(2, 2);
	WSADATA wsaData;
	int err = ::WSAStartup(wVersionRequested, &wsaData);
	
	//TCP
	//���������ʾƵ� TCP UDP��, 0���� �Է��ϸ� �˾Ƽ� �����
	//������ �������ϻ���
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
	//service.sin_addr.s_addr = inet_addr("127.0.0.1"); // �׳� 4����Ʈ���� inet_addr�� �����Լ�
	//naver.com
	::inet_pton(AF_INET, "127.0.0.1", &service.sin_addr);
	service.sin_port = htons(7878); //host to network short ����� �̽�
	//Littl-Endian vs Big-Endian
	// 0x12345678
	// low [78][56][34][12] ��Ʋ�����
	// low [12][34][56][78] �򿣵�� = ��Ʈ��ũ ǥ�� �߰��߰� ���鵵 ���ٺ��ϱ� �ΰ��� ����� �ϳ��� ��� ����ؾ���
	// ������ �Ѱ�����å�� ��� �ϰ����ְ� ����ؾ��Ѵ�.
	// ��κ��� ��Ʋ�����


	//������ �������� ������ �̷����ϴ°�
	if (SOCKET_ERROR == ::connect(socket, (struct sockaddr*)&service, sizeof(SOCKADDR_IN)))
	{
		int errCode = ::WSAGetLastError();
		std::cout << errCode << " : Connect Fail" << std::endl;
		return 0;
	}

	std::cout << "Connected To Server !" << std::endl;

	while (true)
	{
		//std::this_thread::sleep_for(std::chrono::seconds(10));

		char recvBuffer[256] = "Hello World!";				
		
		

		int sendlen = ::send(socket, recvBuffer, sizeof(recvBuffer), 0);

		if (sendlen == SOCKET_ERROR)
		{
			std::cout << "Send Fail" << std::endl;
		}

		std::cout << "Send Data! Len = " << sendlen << std::endl;
		std::cout << "Send Data = " << recvBuffer << std::endl;

		shutdown(socket, SD_RECEIVE);

		int recvlen = ::recv(socket, recvBuffer, sizeof(recvBuffer), 0);

		if (recvlen == SOCKET_ERROR)
		{
			std::cout << "Send Fail" << std::endl;
		}

		std::cout << "Recv Data! Len = " << recvlen << std::endl;
		std::cout << "Recv Data! = " << recvBuffer << std::endl;


		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
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
