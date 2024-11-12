#include "pch.h"
#include "IocpCore.h"
#include "IocpEvent.h"

IocpCore GIocpCore;

IocpCore::IocpCore()
{
	_iocpHandle = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
	ASSERT_CRASH(_iocpHandle != INVALID_HANDLE_VALUE);
}

IocpCore::~IocpCore()
{
	::CloseHandle(_iocpHandle);
}

bool IocpCore::Register(IocpObject* iocpObject)
{
	return ::CreateIoCompletionPort(iocpObject->GetHandle(), _iocpHandle, reinterpret_cast<ULONG_PTR>(iocpObject)/*key*/, 0);
}

bool IocpCore::Dispatch(uint32 timeoutMS)
{
	DWORD numOfBytes = 0;
	IocpObject* iocpObject = nullptr;
	IocpEvent* iocpEvent = nullptr;

	if (::GetQueuedCompletionStatus(_iocpHandle,
		OUT & numOfBytes,
		OUT reinterpret_cast<PULONG_PTR>(&iocpObject),
		OUT reinterpret_cast<LPOVERLAPPED*>(&iocpEvent),
		timeoutMS))
	{
		iocpObject->Dispatch(iocpEvent, numOfBytes);
	}
	else
	{
		int32 errCode = ::WSAGetLastError();

		switch (errCode)
		{
		case WAIT_TIMEOUT:
			return false;
		default:
			// TODO : �α� ���

			break;
		}

	}

	return false;
}
