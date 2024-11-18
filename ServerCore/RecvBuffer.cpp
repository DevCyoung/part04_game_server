#include "pch.h"
#include "RecvBuffer.h"

RecvBuffer::RecvBuffer(int32 bufferSize)
	:_bufferSize(bufferSize)
{
	_capacity = bufferSize * BUFFER_COUNT;
	_buffer.resize(_capacity);
}

RecvBuffer::~RecvBuffer()
{
}

void RecvBuffer::Clean()
{
	int32 dataSize = DataSize();
	if (dataSize == 0)
	{
		_readPos = _writePos = 0;
	}
	else
	{
		//���� ������ ���� 1�� ũ�� �̸��̸�, �����͸� ������ ����.
		if (FreeSize() < _bufferSize)
		{
			::memcpy_s(_buffer.data(), _writePos - _readPos,
				_buffer.data() + _writePos, _writePos - _readPos);

			_writePos -= _readPos;
			_readPos = 0;
		}
	}

}

bool RecvBuffer::OnRead(int32 numOfBytes)
{
	if (numOfBytes > DataSize())
	{
		return false;
	}

	_readPos += numOfBytes;

	return false;
}

bool RecvBuffer::OnWrite(int32 numOfBytes)
{
	if (numOfBytes > FreeSize())
	{
		return false;
	}

	_writePos += numOfBytes;

	return false;
}
