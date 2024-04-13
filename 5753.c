int LuaSettings::l_set_bool(lua_State* L)
{
	NO_MAP_LOCK_REQUIRED;
	LuaSettings* o = checkobject(L, 1);

	std::string key = std::string(luaL_checkstring(L, 2));
	bool value = readParam<bool>(L, 3);

	CHECK_SETTING_SECURITY(L, key);

	o->m_settings->setBool(key, value);

	return 0;
}