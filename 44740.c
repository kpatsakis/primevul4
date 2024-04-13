static void __init parse_floppy_cfg_string(char *cfg)
{
	char *ptr;

	while (*cfg) {
		ptr = cfg;
		while (*cfg && *cfg != ' ' && *cfg != '\t')
			cfg++;
		if (*cfg) {
			*cfg = '\0';
			cfg++;
		}
		if (*ptr)
			floppy_setup(ptr);
	}
}
