#include "Connection.h"
#include <cstdlib>

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
	Slidefuse::Util::luaToBSON(state, toJson, tbl);
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
	collection = Slidefuse::Util::sanitizeCollection(this, collection);

	if (moduleDebug) {
		printf("Inserting into `%s`: %s", collection.c_str(), tbl.toString());
	}

	try {
		conn.insert(collection, tbl);
		std::string e = conn.getLastError();
		if (!e.empty())
			printf("MongoDB Insert Error: %s", e);

	} catch (const mongo::DBException &e) {
		printf("MongoDB Exception: %s", e.what());
	}
}

void Connection::Query(std::string collection, mongo::BSONObj query) {
	collection = Slidefuse::Util::sanitizeCollection(this, collection);
	auto_ptr<mongo::DBClientCursor> cursor = conn.query(collection, query);
	//todo - fix this and finish query shit
}