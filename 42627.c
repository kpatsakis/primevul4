static int get_value(config_fn_t fn, void *data, char *name, unsigned int len)
{
	int c;
	char *value;

	/* Get the full name */
	for (;;) {
		c = get_next_char();
		if (config_file_eof)
			break;
		if (!iskeychar(c))
			break;
		name[len++] = c;
		if (len >= MAXNAME)
			return -1;
	}
	name[len] = 0;
	while (c == ' ' || c == '\t')
		c = get_next_char();

	value = NULL;
	if (c != '\n') {
		if (c != '=')
			return -1;
		value = parse_value();
		if (!value)
			return -1;
	}
	return fn(name, value, data);
}
