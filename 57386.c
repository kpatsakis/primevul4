R_API RConfigNode* r_config_set_i_cb(RConfig *cfg, const char *name, int ivalue, RConfigCallback cb) {
	RConfigNode *node = r_config_set_i (cfg, name, ivalue);
	if (node && (node->setter = cb)) {
		if (!node->setter (cfg->user, node)) {
			return NULL;
		}
	}
	return node;
}
