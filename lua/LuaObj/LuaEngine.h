#pragma once
#include <stdint.h>
#include "LuaStateOwner.h"

class LuaEngine;

class LuaManager
{
public:
	LuaManager(void);
	~LuaManager(void);

	static LuaManager* instance();//ȫ��
	LuaEngine* current();//��ȡ��ǰ�̵߳�lua����,�����ǰ�߳�û��ʹ���򷵻�NULL

private:
	friend class LuaEngine;
	void setCurrent(LuaEngine* engine);
	uint32_t m_key;
};


//lua����
class LuaEngine:public LuaStateOwner
{
public:
	LuaEngine();
	~LuaEngine();

	//������չ

};


/*
int threadFunc(void* d)
{
	LuaEngine engine;

	//֮��������߳���ʹ�û�ȡ��ǰ�̵߳�lua����
	LuaEngine* lua=LuaManager::instance()->current()


}


*/
