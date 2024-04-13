static int get_base_var(char *name)
{
	int baselen = 0;

	for (;;) {
		int c = get_next_char();
		if (config_file_eof)
			return -1;
		if (c == ']')
			return baselen;
		if (isspace(c))
			return get_extended_base_var(name, baselen, c);
		if (!iskeychar(c) && c != '.')
			return -1;
		if (baselen > MAXNAME / 2)
			return -1;
		name[baselen++] = tolower(c);
	}
}
