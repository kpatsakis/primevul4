comics_regex_quote (const gchar *unquoted_string)
{
	const gchar *p;
	GString *dest;

	dest = g_string_new ("'");

	p = unquoted_string;

	while (*p) {
		switch (*p) {
			/* * matches a sequence of 0 or more characters */
			case ('*'):
			/* ? matches exactly 1 charactere */
			case ('?'):
			/* [...]  matches any single character found inside
			 * the brackets. Disabling the first bracket is enough.
			 */
			case ('['):
				g_string_append (dest, "[");
				g_string_append_c (dest, *p);
				g_string_append (dest, "]");
				break;
			/* Because \ escapes regex expressions that we are
			 * disabling for unzip, we need to disable \ too */
			case ('\\'):
				g_string_append (dest, "[\\\\]");
				break;
			/* Escape single quote inside the string */
			case ('\''):
				g_string_append (dest, "'\\''");
				break;
			default:
				g_string_append_c (dest, *p);
				break;
		}
		++p;
	}
	g_string_append_c (dest, '\'');
	return g_string_free (dest, FALSE);
}
