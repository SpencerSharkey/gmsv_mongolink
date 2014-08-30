#ifndef LUABRIDGE_H
#define LUABRIDGE_H

#include "Module.h"
#include "Util.h"

namespace LuaBridge {
	int ToString(lua_State* state);
	int Connect(lua_State* state);
	int Auth(lua_State* state);
	int Insert(lua_State* state);
	int Query(lua_State* state);
	int SetDebug(lua_State* state);
	int ListCollections(lua_State* state);
}

#endif