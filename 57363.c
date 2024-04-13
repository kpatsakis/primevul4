R_API RConfig* r_config_clone(RConfig *cfg) {
	RListIter *iter;
	RConfigNode *node;
	RConfig *c = r_config_new (cfg->user);
	if (!c) {
		return NULL;
	}
	r_list_foreach (cfg->nodes, iter, node) {
		RConfigNode *nn = r_config_node_clone (node);
		ht_insert (c->ht, node->name, nn);
		r_list_append (c->nodes, nn);
		c->n_nodes++;
	}
	c->lock = cfg->lock;
	c->cb_printf = cfg->cb_printf;
	return c;
}
