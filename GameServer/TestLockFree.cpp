#include "pch.h"
#include "TestLockFree.h"

/*----------------------
* First Try
----------------------*/
//struct SListEntry
//{
//	SListEntry* next;
//};
//
//struct SListHeader
//{
//	SListEntry* next = nullptr;
//};
//
//class Data
//{
//public:
//	SListEntry _entry;
//
//	int32 _hp;
//	int32 _mp;
//};
//
//// [ ] [ ] [ ] [ ]
//// [Header]
//
//void InitializeHead(SListHeader* header)
//{
//	header->next = nullptr;
//}
//
////Push
//void PushEntrySList(SListHeader* header, SListEntry* entry)
//{
//	entry->next = header->next;
//	header->next = entry;
//}
//
//SListEntry* PopEntrySList(SListHeader* header)
//{
//	SListEntry* first = header->next;
//
//	if (first != nullptr)
//	{
//		header->next = first->next;
//	}
//
//	return first;
//}

/*----------------------
* Seconds Try
----------------------*/
//struct SListEntry
//{
//	SListEntry* next;
//};
//
//struct SListHeader
//{
//	SListEntry* next = nullptr;
//};
//
//class Data
//{
//public:
//	SListEntry _entry;
//
//	int32 _hp;
//	int32 _mp;
//};
//
//void PushEntrySList(SListHeader* header, SListEntry* entry)
//{	
//	//64bit 지원
//	entry->next = header->next;
//	while (0 == ::InterlockedCompareExchange64( (int64*)&header->next, (int64)entry , (int64)entry->next))
//	{
//	}	
//}
//
//
////삭제 할 수 있는 경우가 문제가 생김
//SListEntry* PopEntrySList(SListHeader* header)
//{
//	SListEntry* expected = header->next;
//
//	// ABA Problem
//	// A상태에서 B상태 그리고 A상태가 되었다.
//
//	// 딱 이상태롤 캡처한상태이다.
//	// [5000] -> [6000] -> [7000]
//	// 그런데 pop pop을함
//	// [7000]
//	// 이때 우연히 5000이들어옴
//	// [5000] -> [7000]
//	// 실행하는게 Header가 5000이라면 6000으로 해줘임
//
//	// 기존락프리스택은 삭제하지않고 붙잡고있었음
//
//	while (expected && 0 == ::InterlockedCompareExchange64((int64*)&header->next, (int64)expected->next, (int64)expected))
//	{
//	}	
//
//	return expected;
//}


/*----------------------
* Thirds Try
----------------------*/

//16바이트 얼라인
DECLSPEC_ALIGN(16)
struct SListEntry
{
	SListEntry* next;
};

DECLSPEC_ALIGN(16)
struct SListHeader
{
	SListHeader()
	{
		alignment = 0;
		region = 0;
	}

	union
	{
		struct
		{
			uint64 alignment;
			uint64 region;			
		} DUMMYSTRUCTNAME;

		struct
		{
			uint64 depth : 16;
			uint64 sequence : 48;
			uint64 reservec : 4;
			uint64 next : 60;
		} HeaderX64;
	};


	SListEntry* next = nullptr;
};

class Data
{
public:
	SListEntry _entry;

	int32 _hp;
	int32 _mp;
};

void InitializeHead(SListHeader* header)
{
	header->alignment = 0;
	header->region = 0;	
}

void PushEntrySList(SListHeader* header, SListEntry* entry)
{
	SListHeader expected = {};
	SListHeader desired = {};

	desired.HeaderX64.next = ((uint64)entry >> 4);

	while (true)
	{
		expected = *header;

		// 이 사이에 변경될 수 있다.

		entry->next = (SListEntry*)(((uint64)expected.HeaderX64.next) << 4);

		desired.HeaderX64.depth = expected.HeaderX64.depth + 1;
		desired.HeaderX64.sequence = expected.HeaderX64.sequence + 1;

		//128 비트를 한번에 해주는얘가있음
		if (::InterlockedCompareExchange128((int64*)header, desired.region, desired.alignment,
			(int64*)&expected) == 1)
		{
			break;
		}
	}
}


//삭제 할 수 있는 경우가 문제가 생김
SListEntry* PopEntrySList(SListHeader* header)
{
	SListHeader expected = {};
	SListHeader desired = {};
	SListEntry* entry = nullptr;

	while (true)
	{
		expected = *header;

		entry = (SListEntry*)(((uint64)expected.HeaderX64.next) << 4);
		if (entry == nullptr)
		{
			break;
		}

		desired.HeaderX64.next = ((uint64)entry->next) >> 4;
		desired.HeaderX64.depth = expected.HeaderX64.depth - 1;
		desired.HeaderX64.sequence = expected.HeaderX64.sequence + 1;

		//if (::InterlockedCompareExchange128(int64*)header, desired, region)
	}

	return nullptr;
}