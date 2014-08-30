#include "LuaBridge.h"
#include <cstdlib>

Connection * fetchConnection(lua_State* state) {
	if (!LUA->IsType(1, GarrysMod::Lua::Type::USERDATA)) {
		LUA->ThrowError("No connection userdata used");
		return NULL;
	}
	return *(Connection**)LUA->GetUserdata(1);
}

int LuaBridge::ToString(lua_State* state)  {
	Connection* conn = fetchConnection(state);
	if (!conn) {
		LUA->PushNil();
	} else {
		LUA->PushString((conn->connUser + "@" + conn->connHostname + "/" + conn->dbName).c_str());
	}
	return 1;
}

int LuaBridge::Connect(lua_State* state) {
	Connection* conn = fetchConnection(state);
	if (!conn) return 0;

	if (LUA->IsType(2, GarrysMod::Lua::Type::STRING)) {
		std::string hostname = LUA->GetString(2);
		if (conn->Connect(hostname)) {
			LUA->PushBool(true);
		} else {
			LUA->ThrowError("Error connecting");
			LUA->PushBool(false);
		}
	} else {
		LUA->ThrowError("Connect() requires hostname");
		return 0;
	}

	return 1;
}

int LuaBridge::Auth(lua_State* state) {
	Connection* conn = fetchConnection(state);
	if (!conn) return 0;

	std::string db = LUA->GetString(2);
	std::string user = LUA->GetString(3);
	std::string pass = LUA->GetString(4);

	if (conn->Auth(db, user, pass)) {
		LUA->PushBool(true);
	} else {
		LUA->PushBool(false);
	}

	return 1;
}

int LuaBridge::Insert(lua_State* state) {
	Connection* conn = fetchConnection(state);
	if (!conn) return 0;

	std::string collection = LUA->GetString(2);

	LUA->GetUserdata(3);
	int toJson = (int)LUA->ReferenceCreate();

	mongo::BSONObj tbl;
	Slidefuse::Util::luaToBSON(state, toJson, tbl);
	conn->Insert(collection, tbl);
	return 0;
}

int LuaBridge::SetDebug(lua_State* state) {
	Connection* conn = fetchConnection(state);
	if (!conn) return 0;

	if (LUA->IsType(2, GarrysMod::Lua::Type::BOOL)) {
		bool newDebug = LUA->GetBool(2);
		conn->moduleDebug = newDebug;
	} else {
		LUA->ThrowError("connection:SetDebug() requires a bool parameter");
	}
	return 0;
}

int LuaBridge::Query(lua_State* state) {
	Connection* conn = fetchConnection(state);
	if (!conn) return 0;

	std::string collection = LUA->GetString(2);
	LUA->GetUserdata(3);
	int queryTable = (int)LUA->ReferenceCreate();

	mongo::BSONObj tbl;
	Slidefuse::Util::luaToBSON(state, queryTable, tbl);
	int resultsTable = conn->Query(state, collection, tbl);

	LUA->ReferencePush(resultsTable);
	
	return 1;
}

int LuaBridge::ListCollections(lua_State* state) {
	Connection* conn = fetchConnection(state);
	if (!conn) return 0;

	int resultsTable = conn->ListCollections(state);

	LUA->ReferencePush(resultsTable);

	return 1;
	
}