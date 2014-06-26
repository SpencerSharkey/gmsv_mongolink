#ifndef LUA_H
#define LUA_H

#include "Module.h"

namespace Lua
{
	namespace Table
	{
		int Create();
		void Add(int iTableReference, const std::string &key, int iTable);
		void Insert(int iTableReference, const std::string &key, GarrysMod::Lua::CFunc value);
		void Insert(int iTableReference, const std::string &key, int value);
		void Insert(int iTableReference, const std::string &key, std::string value);
		void Insert(int iTableReference, int key, std::string value);
	}
}

#endif