#pragma once
#include "LuaCommon.h"


class LuaException
{
public:
	LuaException(const std::string & w):m_what(w){}
	LuaException(const char* w):m_what(w){}
	virtual ~LuaException(){}

	virtual const char* what()
	{
		return m_what.c_str();
	}
private:
	std::string m_what;
};

//��������
typedef int (*LuaErrorHandler) (const char* err);







