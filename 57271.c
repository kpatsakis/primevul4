static int cmd_mv(void *data, const char *input) {
	return r_syscmd_mv (input)? 1: 0;
}
