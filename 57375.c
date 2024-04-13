R_API void r_config_node_free(void *n) {
	RConfigNode *node = (RConfigNode *)n;
	if (!node) {
		return;
	}
	free (node->name);
	free (node->desc);
	free (node->value);
	r_list_free (node->options);
	free (node);
}
