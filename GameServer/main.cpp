#include "pch.h"
#include <thread>
#include <atomic>
#include <mutex>
#include <Windows.h>
#include <assert.h>
#include <future>

atomic<bool> flag;

int main()
{
	flag = false;
	//flag.is_lock_free(); // ~�̾��� ���̾���~ ���̿־���? bool ��ü�� CPU�� ���ؼ� ���������� ó���Ҽ� �ֱ⶧��!
	//���� �����ʿ��� �����͸� ������� �˾Ƽ� ���ɾ���
	//flag = true;

	//flag = true stor�� ������
	flag.store(true, memory_order_seq_cst);

	bool val = flag.load(memory_order_seq_cst);

	// ���� flag ���� prev�� �ְ�, flag ���� ����
	{
		//�̰Ͷ��� �а� ���⸦ ���ÿ��ؾ��� exchagne()
		bool prev = flag.exchange(true); // �����ְ� �����ǰ��� �޾ƿ� �ݵ�� exchange�� �ؾ���

		//bool prev = flag;
		//flag = true;
	}
	
	// CAS (Compare-And-Swap) ���Ǻ� ����
	{
		bool expected = false;
		bool desired = true;
		flag.compare_exchange_strong(expected, desired);

		if (flag == expected)
		{
			expected = flag;
			flag = desired;
			return true;
		}
		else
		{
			expected = flag;
			return false;
		}
	}

}
