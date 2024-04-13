static void config_print_node(RConfig *cfg, RConfigNode *node, const char *pfx, const char *sfx, bool verbose, bool json) {
	char *option;
	bool isFirst;
	RListIter *iter;
	char *es = NULL;

	if (json) {
		if (verbose) {
			cfg->cb_printf ("{");
			cfg->cb_printf ("\"name\":\"%s\",", node->name);
			cfg->cb_printf ("\"value\":");
			config_print_value_json (cfg, node);
			cfg->cb_printf (",\"type\":\"%s\",", r_config_node_type (node));
			es = r_str_escape (node->desc);
			if (es) {
				cfg->cb_printf ("\"desc\":\"%s\",", es);
				free (es);
			}
			cfg->cb_printf ("\"ro\":%s", node->flags & CN_RO ? "true" : "false");
			if (!r_list_empty (node->options)) {
				isFirst = true;
				cfg->cb_printf (",\"options\":[");
				r_list_foreach (node->options, iter, option) {
					es = r_str_escape (option);
					if (es) {
						if (isFirst) {
							isFirst = false;
						} else {
							cfg->cb_printf (",");
						}
						cfg->cb_printf ("\"%s\"", es);
						free (es);
					}
				}
				cfg->cb_printf ("]");
			}
			cfg->cb_printf ("}");
		} else {
			cfg->cb_printf ("\"%s\":", node->name);
			config_print_value_json (cfg, node);
		}
	} else {
		if (verbose) {
			cfg->cb_printf ("%s%s = %s%s %s; %s", pfx,
				node->name, node->value, sfx, 
				node->flags & CN_RO ? "(ro)" : "", 
				node->desc);
			if (!r_list_empty (node->options)) {
				isFirst = true;
				cfg->cb_printf(" [");
				r_list_foreach (node->options, iter, option) {
					if (isFirst) {
						isFirst = false;
					} else {
						cfg->cb_printf(", ");
					}
					cfg->cb_printf("%s", option);
				}
				cfg->cb_printf("]");
			}
			cfg->cb_printf ("\n");
		} else {
			cfg->cb_printf ("%s%s = %s%s\n", pfx,
				node->name, node->value, sfx);
		}
	}
}
