#include "pch.h"

//C++11 ���� ����������� ǥ������ ����
//�ް��ϸ� ����ѹ������ ����
#include <thread>
//nc�¾����� ��������
//�ؿܴ� ������

void HelloThread()
{
	cout << "Hello Thread" << endl;
}

int main()
{
	//HelloGameServer();	

	std::thread t(HelloThread);
	cout << "Hello Main" << endl;

	int32 count = t.hardware_concurrency(); // ���� CPU����
	auto id = t.get_id(); // �����帶�� �ο��Ǵ� ID �ڵ� ID���� �ٸ���?
	t.detach(); // ���μ������� �ڵ�и� ������������ڸ����ϸ� ������������ �˾Ƽ� ���ҽ� ���ŵȴ�(Ŀ�ο�����Ʈ��)
	//������ �и��ϸ� �ڵ�����ؼ� �������� ������ �˾Ƴ���������.
	//������ ���Ӽ������Ҷ� �߾Ⱦ�

	id = t.get_id();

	//std::thread t1;
	//t1 = std::thread(HelloThread);
	//auto id2 = t1.get_id();

	//t1.joinable();
	//������ �����尡 �����ϴ°�?
	//t.join();
	if (t.joinable())
	{
		t.join();
	}

	//�ش簴ü�� �ñ׳λ��¸� OK �ͽñ׳��̸� �ñ׳��϶� ��������(Sleep)
	//t.join();


	//vector<std::thread> v;
	//v.resize(10);


	return 0;
}