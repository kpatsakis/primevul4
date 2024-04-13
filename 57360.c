static void config_print_value_json(RConfig *cfg, RConfigNode *node) {
	const char *val = node->value;
	if (!val) {
		val = "0";
	}
	char *sval = r_str_escape (val);
	if (node->flags & CN_BOOL || node->flags & CN_INT || node->flags & CN_OFFT) {
		if (!strncmp (val, "0x", 2)) {
			ut64 n = r_num_get (NULL, val);
			cfg->cb_printf ("%"PFMT64d, n);
		} else if (r_str_isnumber (val) || !strcmp (val, "true") || !strcmp (val, "false")) {
			cfg->cb_printf ("%s", val);
		} else {
			cfg->cb_printf ("\"%s\"", sval);
		}
	} else {
		cfg->cb_printf ("\"%s\"", sval);
	}
	free (sval);
}
