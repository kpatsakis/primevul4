R_API void r_config_visual_hit_i(RConfig *cfg, const char *name, int delta) {
	RConfigNode *node = r_config_node_get (cfg, name);
	if (node && (node->flags & CN_INT || node->flags & CN_OFFT)) {
		(void)r_config_set_i (cfg, name, r_config_get_i (cfg, name) + delta);
	}
}
