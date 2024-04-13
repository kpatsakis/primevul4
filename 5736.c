LuaSettings::LuaSettings(const std::string &filename, bool write_allowed) :
	m_filename(filename),
	m_is_own_settings(true),
	m_write_allowed(write_allowed)
{
	m_settings = new Settings();
	m_settings->readConfigFile(filename.c_str());
}