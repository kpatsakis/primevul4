int iscsi_extract_key_value(char *textbuf, char **key, char **value)
{
	*value = strchr(textbuf, '=');
	if (!*value) {
		pr_err("Unable to locate \"=\" separator for key,"
				" ignoring request.\n");
		return -1;
	}

	*key = textbuf;
	**value = '\0';
	*value = *value + 1;

	return 0;
}
