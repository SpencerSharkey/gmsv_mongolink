#include "Util.h"

void Slidefuse::Util::luaToBSON(lua_State* state, int toJson, mongo::BSONObj &obj) {

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

void Slidefuse::Util::stackTrace(lua_State* state) {
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