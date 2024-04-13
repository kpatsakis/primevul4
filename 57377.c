R_API RConfigNode* r_config_node_new(const char *name, const char *value) {
	RConfigNode *node;
	if (STRNULL (name)) {
		return NULL;
	}
	node = R_NEW0 (RConfigNode);
	if (!node) {
		return NULL;
	}
	node->name = strdup (name);
	node->value = strdup (value? value: "");
	node->flags = CN_RW | CN_STR;
	node->i_value = r_num_get (NULL, value);
	node->options = r_list_new ();
	return node;
}
