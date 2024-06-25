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
	//flag.is_lock_free(); // ~이없다 락이없다~ 락이왜없서? bool 자체가 CPU에 의해서 원자적으로 처리할수 있기때문!
	//물론 락이필요한 데이터면 아토믹이 알아서 락걸어줌
	//flag = true;

	//flag = true stor와 같은말
	flag.store(true, memory_order_seq_cst);

	bool val = flag.load(memory_order_seq_cst);

	// 이전 flag 값을 prev에 넣고, flag 값을 수정
	{
		//이것또한 읽고 쓰기를 동시에해야함 exchagne()
		bool prev = flag.exchange(true); // 값을넣고 이전의값을 받아옴 반드시 exchange로 해야함

		//bool prev = flag;
		//flag = true;
	}
	
	// CAS (Compare-And-Swap) 조건부 수정
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
