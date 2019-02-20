#pragma once
#include "LuaState.h"


//���ڳ�ʼ����lua
class LuaStateOwner:public LuaState
{
public:

	LuaStateOwner()
		:m_pool(0,false),LuaState(NULL),m_errorHandler(NULL)
	{
		m_type=LST_OWNER;
		m_ls=lua_newstate(luaAlloc,&m_pool);
		luaL_openlibs(m_ls);//��ʼ����
		lua_settop(m_ls,0);//���ջ
	}

	~LuaStateOwner()
	{
		lua_close(m_ls);

		mallinfo info=m_pool.mallinfo();
		m_ls=NULL;
	}

	static void * luaAlloc(void *ud, void *ptr, size_t osize, size_t nsize);//�ڴ����

	MemPool* getMemPool(){return &m_pool;}

	virtual void error(const char* errorMsg,...);

	void setErrorHandler(LuaErrorHandler handler){m_errorHandler=handler;}
private:
	MemPool m_pool;//�ڴ�����
	LuaErrorHandler m_errorHandler;//��������
};