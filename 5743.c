int LuaSettings::l_write(lua_State* L)
{
	NO_MAP_LOCK_REQUIRED;
	LuaSettings* o = checkobject(L, 1);

	if (!o->m_write_allowed) {
		throw LuaError("Settings: writing " + o->m_filename +
				" not allowed with mod security on.");
	}

	bool success = o->m_settings->updateConfigFile(o->m_filename.c_str());
	lua_pushboolean(L, success);

	return 1;
}