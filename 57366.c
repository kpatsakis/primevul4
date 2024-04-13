R_API int r_config_free(RConfig *cfg) {
	if (!cfg) {
		return 0;
	}
	cfg->nodes->free = r_config_node_free; // damn
	r_list_free (cfg->nodes);
	ht_free (cfg->ht);
	free (cfg);
	return 0;
}
