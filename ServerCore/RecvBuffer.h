#pragma once
#include "Types.h"

/*-----------------
* RecvBuffer
-----------------*/
// [r][][][w][][][][][][][][][][][][][][]
// Read Cursor와 Writre Cursor
// r 과 w 사이에 낑겨있는 데이터가 유효한 데이터이다.
// w가 꽊차버리면? 여러가지 처리방법이있음
// 순환 버퍼를 사용하는 방법이있음
// 2번재방법
// rw가 겹치는순간 0번으로 이동
// [][][][r][][w] 와같이 되있으면 어쩔수없이 r~w같이 이동
class RecvBuffer
{
	//요청한 크기보다 10배를 더크게해 read send를 겹치기를 바란다.
	enum { BUFFER_COUNT = 10 };

public:
	RecvBuffer(int32 bufferSize);
	~RecvBuffer();

	void Clean();
	bool OnRead(int32 numOfBytes);
	bool OnWrite(int32 numOfBytes);

	BYTE* ReadPos() { return &_buffer[_readPos]; }
	BYTE* WritePos() { return &_buffer[_writePos]; }
	int32 DataSize() { return _writePos - _readPos; }
	int32 FreeSize() { return _capacity - _writePos; }

private:
	int32	_capacity = 0;
	int32	_bufferSize = 0;
	int32 _readPos = 0;
	int32 _writePos = 0;
	std::vector<BYTE> _buffer;
};

