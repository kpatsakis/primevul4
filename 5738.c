int LuaSettings::l_get_flags(lua_State *L)
{
	NO_MAP_LOCK_REQUIRED;
	LuaSettings *o = checkobject(L, 1);
	std::string key = std::string(luaL_checkstring(L, 2));

	u32 flags = 0;
	auto flagdesc = o->m_settings->getFlagDescFallback(key);
	if (o->m_settings->getFlagStrNoEx(key, flags, flagdesc)) {
		lua_newtable(L);
		int table = lua_gettop(L);
		for (size_t i = 0; flagdesc[i].name; ++i) {
			lua_pushboolean(L, flags & flagdesc[i].flag);
			lua_setfield(L, table, flagdesc[i].name);
		}
		lua_pushvalue(L, table);
	} else {
		lua_pushnil(L);
	}

	return 1;
}