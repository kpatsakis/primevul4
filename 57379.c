R_API int r_config_readonly(RConfig *cfg, const char *key) {
	RConfigNode *n = r_config_node_get (cfg, key);
	if (!n) {
		return false;
	}
	n->flags |= CN_RO;
	return true;
}
