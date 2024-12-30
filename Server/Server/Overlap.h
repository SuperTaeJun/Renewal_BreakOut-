#pragma once
#include "pch.h"


class Overlap {
public:
	WSAOVERLAPPED   _wsa_over;
	IO_type         _op;
	WSABUF         _wsa_buf;
	unsigned		 char   _net_buf[2047];
	int            _target;
public:
	Overlap(IO_type _op, char num_bytes, void* mess) : _op(_op)
	{
		ZeroMemory(&_wsa_over, sizeof(_wsa_over));
		_wsa_buf.buf = reinterpret_cast<char*>(_net_buf);
		_wsa_buf.len = num_bytes;
		memcpy(_net_buf, mess, num_bytes);
	}

	Overlap(IO_type _op) : _op(_op) {}

	Overlap()
	{
		_op = IO_RECV;
	}

	~Overlap()
	{
	}
};
