#pragma once
#include "LuaCommon.h"
#include "LuaObject.h"
#include "LuaStack.h"

class LuaTable:public LuaObject
{
public:
	LuaTable(LuaState* L);
	LuaTable(LuaObjectImpl* impl);
	LuaTable(const LuaObject& rfs);
	LuaTable(const LuaTable& rfs);

	//判断是否是lua表
	bool isValid() const;

	//lua表操作
	LuaObject getTable(const char* key) const;
	LuaObject getTable(lua_Integer idx) const;

	LuaObject operator[](const char* key);
	LuaObject operator[](lua_Integer idx);


	bool setTable(const char* key, const LuaObject& val);
	bool setTable(lua_Integer key, const LuaObject& val);

	inline size_t count()const{return objLen();}

	template<class T>
	bool setTable(const char* key,T val)
	{
#ifdef DUILIB_LUA
		if(isValid())
		{
			lua_State* L=getLuaState();
			lua_pushstring(L,key);//key
			StackOps::Push(getLuaState(),val);
			lua_settable(L,getIndex());
			return true;
		}
#endif // DUILIB_LUA
		return false;
	}

	template<class T>
	bool setTable(lua_Integer key,T val)
	{
#ifdef DUILIB_LUA
		if(isValid())
		{
			lua_State* L=getLuaState();
			lua_pushinteger(L,key);//key
			StackOps::Push(getLuaState(),val);
			lua_settable(L,getIndex());
			return true;
		}
#endif // DUILIB_LUA
		return false;	
	}
};

