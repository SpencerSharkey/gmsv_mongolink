#ifndef CONNECTION_H
#define CONNECTION_H

#include "Module.h"
#include "Util.h"

class Connection {
public:
	std::string dbName;
	int index;
	mongo::DBClientConnection conn;

	std::string connHostname;
	std::string connUser;

	bool moduleDebug;

	bool Connect(std::string hostname);
	bool Auth(std::string db, std::string user, std::string pass);
	void Insert(std::string collection, mongo::BSONObj tbl);
	
	//Query function uses lua state to manage tables during the execution.
	int Query(lua_State* state, std::string collection, mongo::BSONObj query);

	int ListCollections(lua_State* state);
};

namespace connectionLua {
	int ToString(lua_State* state);
	int Connect(lua_State* state);
	int Auth(lua_State* state);
	int Insert(lua_State* state);
	int Query(lua_State* state);
	int SetDebug(lua_State* state);
	int ListCollections(lua_State* state);
}

#endif