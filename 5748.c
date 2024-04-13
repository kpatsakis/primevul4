LuaSettings::~LuaSettings()
{
	if (m_is_own_settings)
		delete m_settings;
}