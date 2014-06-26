#ifndef MODULE_H
#define MODULE_H

#include <iostream>
#include <string>

#ifndef _WIN32
	#include <sys/stat.h>
	#include <sys/types.h>
	#include <unistd.h>
	#define GetCurrentDir getcwd
#else
	#include <direct.h>
	#define GetCurrentDir _getcwd
#endif

#include "GarrysMod/Lua/Interface.h"

extern GarrysMod::Lua::ILuaBase* g_Lua;

#include "mongo/client/dbclient.h"
#include "Bootil/Bootil.h"
#include "Lua.h"
#include "Connection.h"

#endif