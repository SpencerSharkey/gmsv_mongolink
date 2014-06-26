#include "Connection.h"
#include <cstdlib>

void stackTrace(lua_State* state) {
	int i;
	int top = LUA->Top();
	printf("---- Begin Stack ----\n");
	printf("Stack size: %i\n\n", top);
	for (i = top; i >= 1; i--) {
		int t = LUA->GetType(i);
		switch (t) {
			case GarrysMod::Lua::Type::STRING:
				printf("%i -- (%i) ---- `%s'", i, i - (top + 1), LUA->GetString(i));
				break;

			case GarrysMod::Lua::Type::BOOL:
				printf("%i -- (%i) ---- %s", i, i - (top + 1), LUA->GetBool(i) ? "true" : "false");
				break;

			case GarrysMod::Lua::Type::NUMBER:
				printf("%i -- (%i) ---- %g", i, i - (top + 1), LUA->GetNumber(i));
				break;

			default:
				printf("%i -- (%i) ---- %s", i, i - (top + 1), LUA->GetTypeName(LUA->GetType(i)));
				break;
		}
		printf("\n");
	}
	printf("---- End Stack ----\n");
	printf("\n");
}

void luaToBSON(lua_State* state, int toJson, mongo::BSONObj &obj) {

	std::string jsonString;
	LUA->ReferencePush(toJson);
	LUA->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB);
		LUA->GetField(-1, "util");
			LUA->GetField(-1, "TableToJSON");
			LUA->ReferencePush(toJson);
			if (LUA->PCall(1, 1, 0) != 0)
				LUA->ThrowError(LUA->GetString(-1));
			if (!LUA->IsType(-1, GarrysMod::Lua::Type::STRING)) {
				LUA->ThrowError("Function 'util.TableToJSON' must return a string");
			}
			jsonString = LUA->GetString(-1);
			LUA->Pop(1);
		LUA->Pop();
	LUA->Pop();

	LUA->ReferenceFree(toJson);

	obj = mongo::fromjson(jsonString);
}

Connection * fetchConnection(lua_State* state) {
	if (!LUA->IsType(1, GarrysMod::Lua::Type::USERDATA)) {
		LUA->ThrowError("No connection userdata used");
		return NULL;
	}
	return *(Connection**)LUA->GetUserdata(1);
}

int connectionLua::Connect(lua_State* state) {
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

int connectionLua::Auth(lua_State* state) {
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

int connectionLua::Insert(lua_State* state) {
	Connection* conn = fetchConnection(state);
	if (!conn) return 0;

	std::string collection = LUA->GetString(2);

	LUA->GetUserdata(3);
	int toJson = (int)LUA->ReferenceCreate();

	mongo::BSONObj tbl;
	luaToBSON(state, toJson, tbl);
	conn->Insert(collection, tbl);
	return 0;
}

int connectionLua::SetDebug(lua_State* state) {
	Connection* conn = fetchConnection(state);
	if (!conn) return 0;

	if (LUA->IsType(1, GarrysMod::Lua::Type::BOOL)) {
		bool newDebug = LUA->GetBool(1);
		conn->moduleDebug = newDebug;
	} else {
		LUA->ThrowError("connection:SetDebug() requires a bool parameter");
	}
	return 0;
}

bool Connection::Connect(std::string hostname) {
	try {
		conn.connect(hostname);
	} catch (const mongo::DBException &e) {
		printf("MongoDB Exception: %s", e.what());
		return false;
	}
	return true;
}

bool Connection::Auth(std::string db, std::string user, std::string pass) {
	std::string errMsg;
	bool authed = conn.auth(db, user, pass, errMsg, true);
	if (authed) {
		dbName = db;
		return true;
	} else {
		puts(errMsg.c_str());
		return false;
	}
}

void Connection::Insert(std::string collection, mongo::BSONObj tbl) {
	if (collection.find(".") == std::string::npos) {
		collection = dbName + "." + collection;
	}
	printf("Inserting into `%s`: %s", collection.c_str(), tbl.toString());
	try {
		conn.insert(collection, tbl);
	} catch (const mongo::DBException &e) {
		printf("MongoDB Exception: %s", e.what());
	}
}