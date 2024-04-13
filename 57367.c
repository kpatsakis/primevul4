R_API const char* r_config_get(RConfig *cfg, const char *name) {
	RConfigNode *node = r_config_node_get (cfg, name);
	if (node) {
		if (node->getter) {
			node->getter (cfg->user, node);
		}
		cfg->last_notfound = 0;
		if (node->flags & CN_BOOL) {
			return r_str_bool (is_true (node->value));
		}
		return node->value;
	} else {
		eprintf ("r_config_get: variable '%s' not found\n", name);
	}
	cfg->last_notfound = 1;
	return NULL;
}
