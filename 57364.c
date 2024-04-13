R_API const char* r_config_desc(RConfig *cfg, const char *name, const char *desc) {
	RConfigNode *node = r_config_node_get (cfg, name);
	return r_config_node_desc (node, desc);
}
