#include "duipub.h"
#include "LuaFunction.h"
#include "LuaObjectImpl.h"

#ifdef DUILIB_LUA
LuaFunction::LuaFunction(LuaState* L)
	:LuaObject(L)
{
	
}

LuaFunction::LuaFunction(LuaObjectImpl* impl)
	:LuaObject(impl)
{
	
}

LuaFunction::LuaFunction(const LuaObject& rfs)
	:LuaObject(rfs)
{
}

LuaFunction::LuaFunction(const LuaFunction& rfs)
	:LuaObject(rfs)
{
}

bool LuaFunction::isValid()
{
	return getType()==LUA_TFUNCTION;
}
#endif // DUILIB_LUA
