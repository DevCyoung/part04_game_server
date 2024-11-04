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
//	//64bit ����
//	entry->next = header->next;
//	while (0 == ::InterlockedCompareExchange64( (int64*)&header->next, (int64)entry , (int64)entry->next))
//	{
//	}	
//}
//
//
////���� �� �� �ִ� ��찡 ������ ����
//SListEntry* PopEntrySList(SListHeader* header)
//{
//	SListEntry* expected = header->next;
//
//	// ABA Problem
//	// A���¿��� B���� �׸��� A���°� �Ǿ���.
//
//	// �� �̻��·� ĸó�ѻ����̴�.
//	// [5000] -> [6000] -> [7000]
//	// �׷��� pop pop����
//	// [7000]
//	// �̶� �쿬�� 5000�̵���
//	// [5000] -> [7000]
//	// �����ϴ°� Header�� 5000�̶�� 6000���� ������
//
//	// ���������������� ���������ʰ� ������־���
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

//16����Ʈ �����
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

		// �� ���̿� ����� �� �ִ�.

		entry->next = (SListEntry*)(((uint64)expected.HeaderX64.next) << 4);

		desired.HeaderX64.depth = expected.HeaderX64.depth + 1;
		desired.HeaderX64.sequence = expected.HeaderX64.sequence + 1;

		//128 ��Ʈ�� �ѹ��� ���ִ¾갡����
		if (::InterlockedCompareExchange128((int64*)header, desired.region, desired.alignment,
			(int64*)&expected) == 1)
		{
			break;
		}
	}
}


//���� �� �� �ִ� ��찡 ������ ����
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