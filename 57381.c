R_API int r_config_rm(RConfig *cfg, const char *name) {
	RConfigNode *node = r_config_node_get (cfg, name);
	if (node) {
		ht_delete (cfg->ht, node->name);
		r_list_delete_data (cfg->nodes, node);
		cfg->n_nodes--;
		return true;
	}
	return false;
}
