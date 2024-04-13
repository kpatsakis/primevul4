int LuaSettings::l_get_np_group(lua_State *L)
{
	NO_MAP_LOCK_REQUIRED;
	LuaSettings *o = checkobject(L, 1);

	std::string key = std::string(luaL_checkstring(L, 2));
	if (o->m_settings->exists(key)) {
		NoiseParams np;
		o->m_settings->getNoiseParams(key, np);
		push_noiseparams(L, &np);
	} else {
		lua_pushnil(L);
	}

	return 1;
}