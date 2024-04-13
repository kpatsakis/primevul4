void LuaSettings::create(lua_State *L, Settings *settings,
		const std::string &filename)
{
	LuaSettings *o = new LuaSettings(settings, filename);
	*(void **)(lua_newuserdata(L, sizeof(void *))) = o;
	luaL_getmetatable(L, className);
	lua_setmetatable(L, -2);
}