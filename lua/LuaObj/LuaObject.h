#pragma once
#include "LuaCommon.h"
#include "LuaStack.h"

class LuaObjectImpl;
class LuaTable;

class LuaObject
{
public:
	LuaObject(LuaState* L);
	LuaObject(LuaObjectImpl* impl);
	LuaObject(const LuaObject& rfs);
	~LuaObject(void);

	int getIndex()const;


	//�����ж�
	const char* typeName()const;
	int  getType()const;
	bool isNumber()const;
	bool isString()const;
	bool isCFunction()const;
	bool isFunction()const;
	bool isUData()const;
	bool isTable()const;
	bool isNil()const;
	bool isBool()const;
	bool isThread()const;
	bool isNone()const;


	//to
	lua_Number   toNumber()const;
	lua_Integer  toInt()const;
	bool		 toBool()const;
	const char*  toString()const;
	lua_CFunction toCFunction()const;
	void *		 toData()const;
	void *		 checkData(const char* type)const;

	lua_State*   toThread()const;
	const void*  toPointer()const;

	size_t		 objLen()const;//���������Ԫ�ظ������ַ����ĳ��ȣ�userdata��ռ�ֽ�����

	//������
	LuaObject& operator=(const LuaObject& other);

	//�Ƚ�
	bool operator==(lua_Integer n);
	bool operator==(const char* s);



	lua_State* getLuaState();
	LuaState* getCppLuaState();

	bool setMetatable(LuaTable tab);
	LuaTable getMetatable();

	static LuaObject objFromIndex(LuaState* L,int idx);//�ǽӿں���
protected:
	template<class T> friend  class LuaClass;

	LuaObjectImpl * m_ptr;
	friend class LuaObjectImpl;
};


extern LuaObject luaNil;//lua nil

namespace StackOps
{
	inline void Push(lua_State* L, LuaObject value)	
	{
		lua_pushvalue(L,value.getIndex()); 
	}


}
