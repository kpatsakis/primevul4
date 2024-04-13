R_API RConfigNode* r_config_set_cb(RConfig *cfg, const char *name, const char *value, RConfigCallback cb) {
	RConfigNode *node = r_config_set (cfg, name, value);
	if (node && (node->setter = cb)) {
		if (!cb (cfg->user, node)) {
			return NULL;
		}
	}
	return node;
}
