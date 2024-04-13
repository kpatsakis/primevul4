static void push_settings_table(lua_State *L, const Settings *settings)
{
	std::vector<std::string> keys = settings->getNames();
	lua_newtable(L);
	for (const std::string &key : keys) {
		std::string value;
		Settings *group = nullptr;

		if (settings->getNoEx(key, value)) {
			lua_pushstring(L, value.c_str());
		} else if (settings->getGroupNoEx(key, group)) {
			// Recursively push tables
			push_settings_table(L, group);
		} else {
			// Impossible case (multithreading) due to MutexAutoLock
			continue;
		}

		lua_setfield(L, -2, key.c_str());
	}
}