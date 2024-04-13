static inline int checkSettingSecurity(lua_State* L, const std::string &name)
{
	if (ScriptApiSecurity::isSecure(L) && name.compare(0, 7, "secure.") == 0)
		throw LuaError("Attempted to set secure setting.");

	bool is_mainmenu = false;
#ifndef SERVER
	is_mainmenu = ModApiBase::getGuiEngine(L) != nullptr;
#endif
	if (!is_mainmenu && (name == "mg_name" || name == "mg_flags")) {
		errorstream << "Tried to set global setting " << name << ", ignoring. "
			"minetest.set_mapgen_setting() should be used instead." << std::endl;
		infostream << script_get_backtrace(L) << std::endl;
		return -1;
	}

	const char *disallowed[] = {
		"main_menu_script", "shader_path", "texture_path", "screenshot_path",
		"serverlist_file", "serverlist_url", "map-dir", "contentdb_url",
	};
	if (!is_mainmenu) {
		for (const char *name2 : disallowed) {
			if (name == name2)
				throw LuaError("Attempted to set disallowed setting.");
		}
	}

	return 0;
}