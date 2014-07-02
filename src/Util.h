#ifndef UTIL_H
#define UTIL_H

#include "Module.h"
#include "Connection.h"

namespace Slidefuse {
	namespace Util {
		void luaToBSON(lua_State* state, int toJson, mongo::BSONObj &obj);
		int BSONToLua(lua_State* state, mongo::BSONObj &obj);
		int JSONToLua(lua_State* state, std::string jsonString);
		void stackTrace(lua_State* state);
		std::string sanitizeCollection(std::string dbName, std::string collection);
	}
}

#endif