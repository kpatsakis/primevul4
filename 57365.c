R_API int r_config_eval(RConfig *cfg, const char *str) {
	char *ptr, *a, *b, name[1024];
	unsigned int len;
	if (!str || !cfg) {
		return false;
	}
	len = strlen (str) + 1;
	if (len >= sizeof (name)) {
		return false;
	}
	memcpy (name, str, len);
	str = r_str_chop (name);

	if (!str) {
		return false;
	}

	if (str[0] == '\0' || !strcmp (str, "help")) {
		r_config_list (cfg, NULL, 0);
		return false;
	}

	if (str[0] == '-') {
		r_config_rm (cfg, str + 1);
		return false;
	}

	ptr = strchr (str, '=');
	if (ptr) {
		/* set */
		ptr[0] = '\0';
		a = r_str_chop (name);
		b = r_str_chop (ptr + 1);
		(void) r_config_set (cfg, a, b);
	} else {
		char *foo = r_str_chop (name);
		if (foo[strlen (foo) - 1] == '.') {
			r_config_list (cfg, name, 0);
			return false;
		} else {
			/* get */
			const char *str = r_config_get (cfg, foo);
			if (str) {
				cfg->cb_printf ("%s\n",
					(((int) (size_t) str) == 1)? "true": str);
			}
		}
	}
	return true;
}
