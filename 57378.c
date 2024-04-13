R_API const char* r_config_node_type(RConfigNode *node) {
	if (node) {
		int f = node->flags;
		if (f & CN_BOOL) {
			return "bool";
		}
		if (f & CN_STR) {
			return "str";
		}
		if (f & CN_OFFT || f & CN_INT) {
			if (!strncmp (node->value, "0x", 2)) {
				return "addr";
			}
			return "int";
		}
	}
	return "";
}
