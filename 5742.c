int LuaSettings::l_set(lua_State* L)
{
	NO_MAP_LOCK_REQUIRED;
	LuaSettings* o = checkobject(L, 1);

	std::string key = std::string(luaL_checkstring(L, 2));
	const char* value = luaL_checkstring(L, 3);

	CHECK_SETTING_SECURITY(L, key);

	if (!o->m_settings->set(key, value))
		throw LuaError("Invalid sequence found in setting parameters");

	return 0;
}