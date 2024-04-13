R_API void r_config_lock(RConfig *cfg, int l) {
	r_list_sort (cfg->nodes, (RListComparator) cmp);
	cfg->lock = l;
}
