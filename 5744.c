int LuaSettings::l_remove(lua_State* L)
{
	NO_MAP_LOCK_REQUIRED;
	LuaSettings* o = checkobject(L, 1);

	std::string key = std::string(luaL_checkstring(L, 2));

	CHECK_SETTING_SECURITY(L, key);

	bool success = o->m_settings->remove(key);
	lua_pushboolean(L, success);

	return 1;
}