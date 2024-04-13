int LuaSettings::l_get_names(lua_State* L)
{
	NO_MAP_LOCK_REQUIRED;
	LuaSettings* o = checkobject(L, 1);

	std::vector<std::string> keys = o->m_settings->getNames();

	lua_newtable(L);
	for (unsigned int i=0; i < keys.size(); i++)
	{
		lua_pushstring(L, keys[i].c_str());
		lua_rawseti(L, -2, i + 1);
	}

	return 1;
}