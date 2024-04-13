int LuaSettings::l_to_table(lua_State* L)
{
	NO_MAP_LOCK_REQUIRED;
	LuaSettings* o = checkobject(L, 1);

	MutexAutoLock(o->m_settings->m_mutex);
	push_settings_table(L, o->m_settings);
	return 1;
}