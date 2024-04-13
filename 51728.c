static gboolean empty_line(const gchar *line)
{
	while (*line) {
		if (g_ascii_isspace(*line)) {
			line++;
			continue;
		} else {
			break;
		}
	}
	if (*line == '\0')
		return TRUE;
	else
		return FALSE;
}
