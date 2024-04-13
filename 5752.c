int LuaSettings::create_object(lua_State* L)
{
	NO_MAP_LOCK_REQUIRED;
	bool write_allowed = true;
	const char* filename = luaL_checkstring(L, 1);
	CHECK_SECURE_PATH_POSSIBLE_WRITE(L, filename, &write_allowed);
	LuaSettings* o = new LuaSettings(filename, write_allowed);
	*(void **)(lua_newuserdata(L, sizeof(void *))) = o;
	luaL_getmetatable(L, className);
	lua_setmetatable(L, -2);
	return 1;
}