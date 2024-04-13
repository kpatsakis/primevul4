int LuaSettings::l_get_bool(lua_State* L)
{
	NO_MAP_LOCK_REQUIRED;
	LuaSettings* o = checkobject(L, 1);

	std::string key = std::string(luaL_checkstring(L, 2));
	if (o->m_settings->exists(key)) {
		bool value = o->m_settings->getBool(key);
		lua_pushboolean(L, value);
	} else {
		// Push default value
		if (lua_isboolean(L, 3))
			lua_pushboolean(L, readParam<bool>(L, 3));
		else
			lua_pushnil(L);
	}

	return 1;
}