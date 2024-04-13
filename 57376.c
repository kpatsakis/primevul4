R_API RConfigNode* r_config_node_get(RConfig *cfg, const char *name) {
	if (STRNULL (name)) {
		return NULL;
	}
	return ht_find (cfg->ht, name, NULL);
}
