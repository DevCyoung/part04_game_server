#include "pch.h"

//C++11 부터 쓰레드생성이 표준으로 들어옴
//앵간하면 모던한방법으로 구동
#include <thread>
//nc는아직도 윈도우즈
//해외는 리눅스

void HelloThread()
{
	cout << "Hello Thread" << endl;
}

int main()
{
	//HelloGameServer();	

	std::thread t(HelloThread);
	cout << "Hello Main" << endl;

	int32 count = t.hardware_concurrency(); // 논리적 CPU개수
	auto id = t.get_id(); // 쓰레드마다 부여되는 ID 핸들 ID랑은 다를듯?
	t.detach(); // 프로세스에서 핸들분리 스레드시작하자마자하면 스레드죽을때 알아서 리소스 제거된다(커널오브젝트가)
	//하지만 분리하면 핸들로통해서 스레드의 정보를 알아낼수가없다.
	//때문에 게임서버를할땐 잘안씀

	id = t.get_id();

	//std::thread t1;
	//t1 = std::thread(HelloThread);
	//auto id2 = t1.get_id();

	//t1.joinable();
	//실제로 스레드가 존재하는가?
	//t.join();
	if (t.joinable())
	{
		t.join();
	}

	//해당객체가 시그널상태면 OK 넌시그널이면 시그널일때 날깨워줘(Sleep)
	//t.join();


	//vector<std::thread> v;
	//v.resize(10);


	return 0;
}