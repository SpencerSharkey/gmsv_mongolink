#include "Connection.h"
#include <cstdlib>

Connection * fetchConnection(lua_State* state) {
	if (!LUA->IsType(1, GarrysMod::Lua::Type::USERDATA)) {
		LUA->ThrowError("No connection userdata used");
		return NULL;
	}
	return *(Connection**)LUA->GetUserdata(1);
}

int connectionLua::ToString(lua_State* state)  {
	Connection* conn = fetchConnection(state);
	if (!conn) {
		LUA->PushNil();
	} else {
		LUA->PushString((conn->connUser + "@" + conn->connHostname + "/" + conn->dbName).c_str());
	}
	return 1;
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

	if (LUA->IsType(2, GarrysMod::Lua::Type::BOOL)) {
		bool newDebug = LUA->GetBool(2);
		conn->moduleDebug = newDebug;
	} else {
		LUA->ThrowError("connection:SetDebug() requires a bool parameter");
	}
	return 0;
}

int connectionLua::Query(lua_State* state) {
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

int connectionLua::ListCollections(lua_State* state) {
	Connection* conn = fetchConnection(state);
	if (!conn) return 0;

	int resultsTable = conn->ListCollections(state);

	LUA->ReferencePush(resultsTable);

	return 1;
	
}

bool Connection::Connect(std::string hostname) {
	connHostname = hostname;
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
		connUser = user;
		return true;
	} else {
		puts(errMsg.c_str());
		return false;
	}
}

void Connection::Insert(std::string collection, mongo::BSONObj tbl) {
	collection = Slidefuse::Util::sanitizeCollection(dbName, collection);

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

int Connection::Query(lua_State* state, std::string collection, mongo::BSONObj query) {
	collection = Slidefuse::Util::sanitizeCollection(dbName, collection);

	LUA->CreateTable();
	int resultTable = LUA->ReferenceCreate();

	try {
		std::auto_ptr<mongo::DBClientCursor> cursor = conn.query(collection, query);		
		int i = 1;
		while (cursor->more()) {
			int documentTable = Slidefuse::Util::JSONToLua(state, cursor->next().jsonString());

			LUA->ReferencePush(resultTable);
				LUA->PushNumber(i);
				LUA->ReferencePush(documentTable);
			LUA->SetTable(-3);

			i++;
		}

	} catch (const mongo::DBException &e) {
		printf("MongoDB Query Exception: %s", e.what());
	}

	return resultTable;

}

int Connection::ListCollections(lua_State* state) {
	std::list<std::string> listed = conn.getCollectionNames(dbName);

	LUA->CreateTable();
	int resultTable = LUA->ReferenceCreate();
	int index = 1;
	for (std::list<std::string>::iterator i = listed.begin(); i != listed.end(); ++i) {
		LUA->ReferencePush(resultTable);
			LUA->PushNumber(index);
			LUA->PushString(i->c_str());
		LUA->SetTable(-3);

		index++;
	}

	return resultTable;
}