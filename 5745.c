int LuaSettings::l_set_np_group(lua_State *L)
{
	NO_MAP_LOCK_REQUIRED;
	LuaSettings *o = checkobject(L, 1);

	std::string key = std::string(luaL_checkstring(L, 2));
	NoiseParams value;
	read_noiseparams(L, 3, &value);

	CHECK_SETTING_SECURITY(L, key);

	o->m_settings->setNoiseParams(key, value);

	return 0;
}