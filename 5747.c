int LuaSettings::l_get(lua_State* L)
{
	NO_MAP_LOCK_REQUIRED;
	LuaSettings* o = checkobject(L, 1);

	std::string key = std::string(luaL_checkstring(L, 2));
	if (o->m_settings->exists(key)) {
		std::string value = o->m_settings->get(key);
		lua_pushstring(L, value.c_str());
	} else {
		lua_pushnil(L);
	}

	return 1;
}