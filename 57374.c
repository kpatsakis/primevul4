R_API const char* r_config_node_desc(RConfigNode *node, const char *desc) {
	if (node) {
		if (desc) {
			free (node->desc);
			node->desc = strdup (desc);
		}
		return node->desc;
	}
	return NULL;
}
