#ifndef UTIL_H
#define UTIL_H

#include "Module.h"

namespace Slidefuse {
	namespace Util {
		void luaToBSON(lua_State* state, int toJson, mongo::BSONObj &obj);
		void BSONToLua(lua_State* state, mongo::BSONObj &obj);
		void stackTrace(lua_State* state);
		std::string sanitizeCollection(Connection conn, std::string collection);
	}
}

#endif