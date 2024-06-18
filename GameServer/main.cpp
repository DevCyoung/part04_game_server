#include "pch.h"
#include <thread>
#include <atomic>
#include <mutex>
#include <Windows.h>
#include <assert.h>
#include <future>

int32 buffer[10'000][10'000];

int main()
{
	memset(buffer, 0, sizeof(buffer));

	int arr1[3][1];
	int arr2[3];

	int32 size1 = sizeof(arr1);
	int32 size2 = sizeof(arr2);

	{
		int64 sum = 0;
		uint64 start = GetTickCount64();
		for (int32 i = 0; i < 10'000; ++i)
		{
			for (int32 j = 0; j < 10'00; ++j) 
			{
				sum += buffer[i][j];
			}
		}
		uint64 end = GetTickCount64();
		cout << "Elapsed Tick 1 " << end - start << endl;
	}

	{
		int64 sum = 0;
		uint64 start = GetTickCount64();
		for (int32 i = 0; i < 10'000; ++i)
		{
			for (int32 j = 0; j < 10'00; ++j)
			{
				sum += buffer[j][i];
			}
		}
		uint64 end = GetTickCount64();
		cout << "Elapsed Tick 2 " << end - start << endl;
	}
}