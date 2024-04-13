static bool startswith(const char *line, const char *pref)
{
	if (strncmp(line, pref, strlen(pref)) == 0)
		return true;
	return false;
}
