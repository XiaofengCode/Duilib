#pragma once
#include "../LuaObj/Luaobj.h"


namespace lbind
{

	LuaObject toLua(LuaState& L,RECT rc);
	LuaObject toLua(LuaState& L,SIZE rc);
	LuaObject toLua(LuaState& L,POINT rc);

	RECT toRect(const LuaTable& obj);
	SIZE toSize(const LuaTable& obj);
	POINT toPoint(const LuaTable& obj);
}