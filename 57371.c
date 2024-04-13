R_API void r_config_list(RConfig *cfg, const char *str, int rad) {
	RConfigNode *node;
	RListIter *iter;
	const char *sfx = "";
	const char *pfx = "";
	int len = 0;
	bool verbose = false;
	bool json = false;
	bool isFirst = false;

	if (!STRNULL (str)) {
		str = r_str_chop_ro (str);
		len = strlen (str);
		if (len > 0 && str[0] == 'j') {
			str++;
			len--;
			json = true;
			rad = 'J';
		}
		if (len > 0 && str[0] == ' ') {
			str++;
			len--;
		}
		if (strlen (str) == 0) {
			str = NULL;
			len = 0;
		}
	}

	switch (rad) {
	case 1:
		pfx = "\"e ";
		sfx = "\"";
	/* fallthrou */
	case 0:
		r_list_foreach (cfg->nodes, iter, node) {
			if (!str || (str && (!strncmp (str, node->name, len)))) {
				config_print_node (cfg, node, pfx, sfx, verbose, json);
			}
		}
		break;
	case 2:
		r_list_foreach (cfg->nodes, iter, node) {
			if (!str || (str && (!strncmp (str, node->name, len)))) {
				if (!str || !strncmp (str, node->name, len)) {
					cfg->cb_printf ("%20s: %s\n", node->name,
						node->desc? node->desc: "");
				}
			}
		}
		break;
	case 'v':
		verbose = true;
		r_list_foreach (cfg->nodes, iter, node) {
			if (!str || (str && (!strncmp (str, node->name, len)))) {
				config_print_node (cfg, node, pfx, sfx, verbose, json);
			}
		}
		break;
	case 'q':
		r_list_foreach (cfg->nodes, iter, node) {
			cfg->cb_printf ("%s\n", node->name);
		}
		break;
	case 'J':
		verbose = true;
	/* fallthrou */
	case 'j':
		json = true;
		isFirst = true;
		if (verbose) {
			cfg->cb_printf ("[");
		} else {
			cfg->cb_printf ("{");
		}
		r_list_foreach (cfg->nodes, iter, node) {
			if (!str || (str && (!strncmp (str, node->name, len)))) {
				if (!str || !strncmp (str, node->name, len)) {
					if (isFirst) {
						isFirst = false;
					} else {
						cfg->cb_printf (",");
					}
					config_print_node (cfg, node, pfx, sfx, verbose, json);
				}
			}
		}
		if (verbose) {
			cfg->cb_printf ("]\n");
		} else {
			cfg->cb_printf ("}\n");
		}
		break;
	}
}
