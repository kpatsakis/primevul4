R_API ut64 r_config_get_i(RConfig *cfg, const char *name) {
	RConfigNode *node = r_config_node_get (cfg, name);
	if (node) {
		if (node->getter) {
			node->getter (cfg->user, node);
		}
		if (node->i_value || !strcmp (node->value, "false")) {
			return node->i_value;
		}
		return (ut64) r_num_math (cfg->num, node->value);
	}
	return (ut64) 0LL;
}
