R_API RConfig* r_config_new(void *user) {
	RConfig *cfg = R_NEW0 (RConfig);
	if (!cfg) {
		return NULL;
	}
	cfg->ht = ht_new (NULL, _ht_node_free_kv, NULL);
	cfg->nodes = r_list_newf ((RListFree)r_config_node_free);
	if (!cfg->nodes) {
		R_FREE (cfg);
		return NULL;
	}
	cfg->user = user;
	cfg->num = NULL;
	cfg->n_nodes = 0;
	cfg->lock = 0;
	cfg->cb_printf = (void *) printf;
	return cfg;
}
