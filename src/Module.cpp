#include "Module.h"

GarrysMod::Lua::ILuaBase* g_Lua;

int references;
int refid;

int gmod_About(lua_State* state) {
	puts("Mongolink by Slidefuse LLC - v0.0.1");
	return 0;
}

int gmod_MongoConnection(lua_State* state) {
	Connection** connection;
	Connection* connection_object;

	connection_object = new Connection;
	
	connection_object->index = refid;

	connection = (Connection**)LUA->NewUserdata(sizeof(Connection**));
	*connection = connection_object;

	LUA->CreateMetaTableType("MongoConnection", 60293);

	LUA->SetMetaTable(-2);

	return 1;
}

GMOD_MODULE_OPEN() {

	// Create connection metatable
	LUA->CreateMetaTableType("MongoConnection", 60293);
		LUA->Push(-1);
		LUA->SetField(-2, "__index");

		LUA->PushCFunction(LuaBridge::ToString);
		LUA->SetField(-2, "__tostring");

		LUA->PushCFunction(LuaBridge::Connect);
		LUA->SetField(-2, "Connect");

		LUA->PushCFunction(LuaBridge::Auth);
		LUA->SetField(-2, "Auth");

		LUA->PushCFunction(LuaBridge::Insert);
		LUA->SetField(-2, "Insert");

		LUA->PushCFunction(LuaBridge::Query);
		LUA->SetField(-2, "Query");

		LUA->PushCFunction(LuaBridge::ListCollections);
		LUA->SetField(-2, "ListCollections");

		LUA->PushCFunction(LuaBridge::SetDebug);
		LUA->SetField(-2, "SetDebug");

	LUA->Pop();
	
	//Setup our global identifier
	LUA->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB);
		LUA->CreateTable();
			LUA->PushCFunction(gmod_MongoConnection);
			LUA->SetField(-2, "New");

			LUA->PushCFunction(gmod_About);
			LUA->SetField(-2, "About");
		LUA->SetField(-2, "mongo");
	LUA->Pop();
	
	return 0;
}

GMOD_MODULE_CLOSE() {
	return 0;
}