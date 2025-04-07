#include "pch.h"
#include "RecvBuffer.h"

//--------------
//	RecvBuffer
//--------------

RecvBuffer::RecvBuffer(int32 bufferSize) : _bufferSize(bufferSize)
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
		// 읽기, 쓰기 커서 위치가 같음
		_readPos = _writePos = 0;
	}
	else
	{
		// 여유 공간이 버퍼 크기 1개 미만일 때 앞으로 당김
		if(FreeSize() < _bufferSize)
		{
			::memcpy(&_buffer[0], &_buffer[_readPos], dataSize);
			_readPos = 0;
			_writePos = dataSize;
		}
	}
}

bool RecvBuffer::OnRead(int32 numOfBytes)
{
	if (numOfBytes > DataSize())
		return false;

	_readPos += numOfBytes;

	return true;
}

bool RecvBuffer::OnWrite(int32 numOfBytes)
{
	if (numOfBytes > FreeSize())
		return false;

	_writePos += numOfBytes;

	return true;
}
