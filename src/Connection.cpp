#include "Connection.h"
#include <cstdlib>

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